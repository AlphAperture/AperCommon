/*
 * Copyright © 2019 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of λcommon.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/lambdacommon/system/uri.h"
#include "../../include/lambdacommon/system/system.h"
#include "../../include/lambdacommon/exceptions/exceptions.h"
#include <sstream>
#include <regex>

namespace lambdacommon
{
    namespace uri
    {
        SchemeType LAMBDACOMMON_API get_scheme_type_by_string(const std::string& scheme) {
            if (lstring::equals_ignore_case(scheme, "file"))
                return FILE;
            else if (lstring::equals_ignore_case(scheme, "ftp"))
                return FTP;
            else if (lstring::equals_ignore_case(scheme, "gopher"))
                return GOPHER;
            else if (lstring::equals_ignore_case(scheme, "http"))
                return HTTP;
            else if (lstring::equals_ignore_case(scheme, "https"))
                return HTTPS;
            else if (lstring::equals_ignore_case(scheme, "ws"))
                return WS;
            else if (lstring::equals_ignore_case(scheme, "wss"))
                return WSS;
            else
                return OTHER;
        }

        port_t LAMBDACOMMON_API get_scheme_default_port(SchemeType scheme) {
            switch (scheme) {
                case FILE:
                    return 0;
                case FTP:
                    return 21;
                case GOPHER:
                    return 70;
                case HTTP:
                    return 80;
                case HTTPS:
                    return 443;
                case WS:
                    return 80;
                case WSS:
                    return 443;
                case OTHER:
                    return 0;
                default:
                    return 0;
            }
        }

        bool LAMBDACOMMON_API is_scheme_type_non_file_special(SchemeType scheme) {
            return scheme != FILE && scheme != OTHER;
        }

        /*
         * URI
         */

        std::map<std::string, std::string> fix_queries(const std::map<std::string, std::string>& queries) {
            std::map<std::string, std::string> new_queries;
            for (const auto& query : queries)
                new_queries[lstring::replace_all(query.first, " ", "%20")] = lstring::replace_all(query.second, " ", "%20");
            return new_queries;
        };

        URI::URI(std::string scheme, std::string username, std::string password, Address address, std::string path, const std::map<std::string, std::string>& queries,
                 std::string fragment)
                : AbstractPath(std::move(path)),
                  _scheme(std::move(scheme)),
                  _username(std::move(username)),
                  _password(std::move(password)),
                  _address(std::move(address)),
                  _queries(std::move(fix_queries(queries))),
                  _fragment(std::move(fragment)) {}

        URI::URI(const URI& url) = default;

        URI::URI(URI&& url) noexcept : AbstractPath(std::move(url)), _scheme(std::move(url._scheme)), _username(std::move(url._username)), _password(std::move(url._password)),
                                       _address(std::move(url._address)), _queries(std::move(url._queries)), _fragment(std::move(url._fragment)) {}

        const std::string& URI::get_scheme() const {
            return _scheme;
        }

        const std::string& URI::get_username() const {
            return _username;
        }

        const std::string& URI::get_password() const {
            return _password;
        }

        void URI::set_username_and_password(std::string username, std::string password) {
            if (username.empty() || password.empty()) {
                _username = "";
                _password = "";
            } else {
                _username = std::move(username);
                _password = std::move(password);
            }
        }

        const Address& URI::get_address() const {
            return _address;
        }

        void URI::set_address(Address address) {
            if (address.get_host().empty() && is_scheme_type_non_file_special(get_scheme_type()))
                throw std::invalid_argument("Hostname cannot be null for this scheme.");

            _address = std::move(address);
        }

        std::map<std::string, std::string> URI::get_queries() const {
            return _queries;
        }

        void URI::set_queries(const std::map<std::string, std::string>& queries) {
            _queries = std::move(fix_queries(queries));
        }

        bool URI::has_query(const std::string& query) const {
            return _queries.count(query) > 0;
        }

        std::string URI::get_query(const std::string& query) const {
            return _queries.at(query);
        }

        const std::string& URI::get_fragment() const {
            return _fragment;
        }

        void URI::set_fragment(std::string fragment) {
            _fragment = std::move(fragment);
        }

        std::string URI::to_string(char delimiter) const {
            std::ostringstream oss;

            // Write the scheme.
            if (!_scheme.empty())
                oss << _scheme << ':';

            if (!_address.is_empty()) {
                oss << "//";
                // Write the username and the password.
                if (!_username.empty() && !_password.empty())
                    oss << _username << ':' << _password << '@';

                // Write the address.
                oss << _address.to_string();
            } else if (_scheme == "file")
                oss << "//";

            // Write the path.
            if (!_path.empty())
                oss << (!this->is_absolute() ? "/" : "") << AbstractPath::to_string(delimiter);

            // Write the queries.
            if (!_queries.empty()) {
                oss << '?';
                std::string tmp_separator;
                for (const auto& query : _queries) {
                    oss << tmp_separator << query.first;
                    if (!query.second.empty())
                        oss << '=' << query.second;

                    tmp_separator = "&";
                }
            }

            // Write the fragment.
            if (!_fragment.empty())
                oss << '#' << _fragment;

            return oss.str();
        }

        void URI::open_in_system() const {
            system::open_uri(to_string('/'));
        }

        URI& URI::operator=(const URI& other) {
            if (this != &other) {
                if (other._path != _path)
                    _path = other._path;
                if (other._scheme != _scheme)
                    _scheme = other._scheme;
                if (other._username != _username)
                    _username = other._username;
                if (other._password != _password)
                    _password = other._password;
                if (other._address != _address)
                    _address = other._address;
                if (other._queries != _queries)
                    _queries = other._queries;
                if (other._fragment != _fragment)
                    _fragment = other._fragment;
            }
            return *this;
        }

        URI& URI::operator=(URI&& url) noexcept {
            if (this != &url) {
                _path = std::move(url._path);
                _scheme = std::move(url._scheme);
                _username = std::move(url._username);
                _password = std::move(url._password);
                _address = std::move(url._address);
                _queries = std::move(url._queries);
                _fragment = std::move(url._fragment);
            }
            return *this;
        }

        bool URI::operator==(const URI& url) {
            return _scheme == url._scheme && _username == url._username && _password == url._password && _address == url._address && _path == url._path &&
                   _queries == url._queries && _fragment == url._fragment;
        }

        bool URI::operator!=(const URI& url) {
            return _scheme != url._scheme || _username != url._username || _password != url._password || _address != url._address || _path != url._path ||
                   _queries != url._queries || _fragment != url._fragment;
        }

        URI URI::operator/(const URI& uri) {
            URI new_uri(*this);
            new_uri.append(uri._path);
            return new_uri;
        }

        URI URI::operator/(const AbstractPath& path) {
            URI new_uri(*this);
            new_uri.append(path);
            return new_uri;
        }

        URI LAMBDACOMMON_API from_file_path(fs::path path) {
            if (!path.is_absolute())
                path = path.to_absolute();

            return {"file", "", "", Address::EMPTY, path.to_generic_string()};
        }

        URI LAMBDACOMMON_API from_string(const std::string& url) {
            if (url.empty())
                throw std::invalid_argument("URI cannot be empty.");

            auto scheme_separator = url.find_first_of("://");
            if (scheme_separator == std::string::npos) {
                scheme_separator = url.find_first_of(':');
                if (scheme_separator == std::string::npos)
                    throw ParseException("Cannot parse uri '" + url + "': cannot find the scheme separator (':' or '://')!");
            }
            auto scheme = url.substr(0, scheme_separator);
            auto tmp_uri = url.substr(scheme_separator + 3, url.size());

            auto auth_separator = tmp_uri.find_first_of('@');
            std::string username;
            std::string password;
            if (auth_separator != std::string::npos) {
                auto user_and_pwd = tmp_uri.substr(0, auth_separator);
                auto split = lstring::split(user_and_pwd, ':');
                if (split.size() == 1)
                    throw ParseException("Cannot parse uri '" + url + "': missing username or password!");
                username = split[0];
                password = split[1];

                tmp_uri = tmp_uri.substr(auth_separator + 1, url.size());
            }

            auto path_separator = tmp_uri.find_first_of('/');
            auto query_separator = tmp_uri.find_first_of('?');
            auto fragment_separator = tmp_uri.find_first_of('#');

            if ((query_separator != std::string::npos && path_separator > query_separator) || (fragment_separator != std::string::npos && path_separator > fragment_separator))
                throw ParseException("Cannot parse uri '" + url + "', malformed URI: path is before query and fragment!");

            std::string tmp_address;

            std::string path;
            if (path_separator != std::string::npos) {
                tmp_address = tmp_uri.substr(0, path_separator);
                if (query_separator != std::string::npos || fragment_separator != std::string::npos) {
                    if (query_separator < fragment_separator || fragment_separator == std::string::npos)
                        path = tmp_uri.substr(path_separator + 1, (query_separator - path_separator - 1));
                    else
                        path = tmp_uri.substr(path_separator + 1, (fragment_separator - path_separator - 1));
                } else
                    path = tmp_uri.substr(path_separator);
            }

            std::map<std::string, std::string> queries;
            if (query_separator != std::string::npos) {
                std::string str_queries;
                if (fragment_separator > query_separator) {
                    str_queries = tmp_uri.substr(query_separator + 1, (fragment_separator - query_separator - 1));
                    if (tmp_address.empty())
                        tmp_address = tmp_uri.substr(0, query_separator);
                } else
                    str_queries = tmp_uri.substr(query_separator + 1, tmp_uri.size());

                if (!str_queries.empty()) {
                    auto splitted_queries = lstring::split(str_queries, '&');
                    for (const auto& query : splitted_queries) {
                        size_t j;
                        if ((j = query.find_first_of('=')) != std::string::npos)
                            queries[query.substr(0, j)] = query.substr(j + 1, query.size());
                        else
                            queries[query] = "";
                    }
                }
            }

            std::string fragment;
            if (fragment_separator != std::string::npos) {
                if (query_separator > fragment_separator) {
                    fragment = tmp_uri.substr(fragment_separator + 1, (query_separator - fragment_separator - 1));
                    if (tmp_address.empty())
                        tmp_address = tmp_uri.substr(0, fragment_separator);
                } else
                    fragment = tmp_uri.substr(fragment_separator + 1, tmp_uri.size());
            }

            Address address = Address::EMPTY;
            if (!tmp_address.empty()) {
                auto address_separator = tmp_address.find_last_of(':');
                auto end_ipv6 = tmp_address.find_last_of(']');
                std::string host;
                if (end_ipv6 != std::string::npos && end_ipv6 > address_separator)
                    host = tmp_address;
                else
                    host = tmp_address.substr(0, address_separator);
                host = lstring::replace_all(lstring::replace_all(host, "[", ""), "]", "");
                port_t port = 0;
                if (address_separator != std::string::npos && address_separator > end_ipv6) {
                    auto opt_port = lstring::parse_int(tmp_address.substr(address_separator + 1, tmp_address.size()));
                    if (!opt_port)
                        throw ParseException("Cannot parse uri '" + url + "': invalid port!");
                    else port = static_cast<port_t>(*opt_port);
                }
                address = {std::move(host), port};
            }

            URI result_uri{std::move(scheme), std::move(username), std::move(password), std::move(address), std::move(path), queries, std::move(fragment)};

            if (result_uri.get_address().is_empty() && is_scheme_type_non_file_special(result_uri.get_scheme_type()))
                throw ParseException("Cannot parse uri '" + url + "': the scheme doesn't support empty address!");

            return result_uri;
        }
    }
}
