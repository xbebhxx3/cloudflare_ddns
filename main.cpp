#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <algorithm>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

struct Config {
    std::string cfkey;
    std::string cfuser;
    std::string zone_name;
    std::string record_name;
    int ttl;
    bool update_ipv4;
    bool update_ipv6;
    std::vector<std::string> ipv4_services;
    std::vector<std::string> ipv6_services;
};

Config load_config(const std::string& path) {
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("无法打开配置文件: " + path);
    json j;
    file >> j;
    return Config{
        j["cfkey"],
        j["cfuser"],
        j["zone_name"],
        j["record_name"],
        j["ttl"],
        j.value("update_ipv4", true),
        j.value("update_ipv6", true),
        j.value("ipv4_services", std::vector<std::string>{}),
        j.value("ipv6_services", std::vector<std::string>{})
    };
}

size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string curl_get(const std::string& url, const std::string& ip_version = "") {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (ip_version == "-4") curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        if (ip_version == "-6") curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

std::string get_public_ip(const std::vector<std::string>& urls, const std::string& ip_version = "") {
    for (const auto& url : urls) {
        try {
            std::string ip = curl_get(url, ip_version);
            if (!ip.empty()) {
                ip.erase(std::remove(ip.begin(), ip.end(), '\n'), ip.end());
                return ip;
            }
        } catch (...) {
            continue;
        }
    }
    return "";
}

std::string get_json_id(const std::string& url, const Config& cfg) {
    std::string response;
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("X-Auth-Email: " + cfg.cfuser).c_str());
    headers = curl_slist_append(headers, ("X-Auth-Key: " + cfg.cfkey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    try {
        auto j = json::parse(response);
        if (j.contains("result") && !j["result"].empty())
            return j["result"][0]["id"];
    } catch (...) {
        std::cerr << "解析 ID 失败，返回内容为: " << response << std::endl;
    }
    return "";
}

void update_dns_record(const std::string& id, const std::string& type, const std::string& ip, const Config& cfg, const std::string& zone_id) {
    std::string response;
    CURL* curl = curl_easy_init();
    if (!curl) return;

    std::string url = "https://api.cloudflare.com/client/v4/zones/" + zone_id + "/dns_records/" + id;
    json payload = {
        {"type", type},
        {"name", cfg.record_name},
        {"content", ip},
        {"ttl", cfg.ttl}
    };
    std::string payload_str = payload.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("X-Auth-Email: " + cfg.cfuser).c_str());
    headers = curl_slist_append(headers, ("X-Auth-Key: " + cfg.cfkey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    try {
        auto res = json::parse(response);
        if (res["success"]) {
            std::cout << type << " 记录已成功更新为 " << ip << std::endl;
        } else {
            std::cerr << "更新失败: " << response << std::endl;
        }
    } catch (...) {
        std::cerr << "JSON 解析错误: " << response << std::endl;
    }
}

int main() {
    try {
        Config cfg = load_config("config.json");

        std::string ip4, ip6;

        if (cfg.update_ipv4) {
            ip4 = std::async(std::launch::async, get_public_ip, cfg.ipv4_services, "-4").get();
            if (!ip4.empty())
                std::cout << "IPv4: " << ip4 << std::endl;
            else
                std::cout << "未能获取到 IPv4 地址。" << std::endl;
        }

        if (cfg.update_ipv6) {
            ip6 = std::async(std::launch::async, get_public_ip, cfg.ipv6_services, "-6").get();
            if (!ip6.empty())
                std::cout << "IPv6: " << ip6 << std::endl;
            else
                std::cout << "未能获取到 IPv6 地址。" << std::endl;
        }

        std::string zone_id = get_json_id("https://api.cloudflare.com/client/v4/zones?name=" + cfg.zone_name, cfg);
        if (zone_id.empty()) {
            std::cerr << "获取 Zone ID 失败。" << std::endl;
            return 1;
        }

        if (cfg.update_ipv4 && !ip4.empty()) {
            std::string a_id = get_json_id("https://api.cloudflare.com/client/v4/zones/" + zone_id + "/dns_records?name=" + cfg.record_name + "&type=A", cfg);
            if (!a_id.empty())
                update_dns_record(a_id, "A", ip4, cfg, zone_id);
            else
                std::cerr << "请求失败，请检查是否已设置记录" << std::endl;
        }

        if (cfg.update_ipv6 && !ip6.empty()) {
            std::string aaaa_id = get_json_id("https://api.cloudflare.com/client/v4/zones/" + zone_id + "/dns_records?name=" + cfg.record_name + "&type=AAAA", cfg);
            if (!aaaa_id.empty())
                update_dns_record(aaaa_id, "AAAA", ip6, cfg, zone_id);
            else
                std::cerr << "请求失败，请检查是否已设置记录" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}