
#include "iceoryx_dds/gateway/toml_gateway_config_parser.hpp"
#include "iceoryx_dds/internal/log/logging.hpp"

#include <regex>

iox::cxx::expected<iox::dds::GatewayConfig, iox::dds::TomlGatewayConfigParseError>
iox::dds::TomlGatewayConfigParser::parse()
{
    return iox::dds::TomlGatewayConfigParser::parse(iox::dds::defaultConfigFilePath);
}

iox::cxx::expected<iox::dds::GatewayConfig, iox::dds::TomlGatewayConfigParseError>
iox::dds::TomlGatewayConfigParser::parse(ConfigFilePathString_t path)
{
    GatewayConfig config;

    // Set defaults if no path provided.
    if (path.size() == 0)
    {
        config.setDefaults();
        return iox::cxx::success<iox::dds::GatewayConfig>(config);
    }

    LogInfo() << "[TomlGatewayConfigParser] Loading gateway config at: " << path;

    // Load the file
    auto parsedToml = cpptoml::parse_file(path.c_str());
    auto result = validate(*parsedToml);
    if(result.has_error())
    {
        LogWarn() << "[TomlGatewayConfigParser] Unable to parse configuration file";
        return iox::cxx::error<TomlGatewayConfigParseError>(result.get_error());
    }

    // Prepare config object
    auto serviceArray = parsedToml->get_table_array("services");
    for(const auto& service : *serviceArray)
    {
        auto name = service->get_as<std::string>("service");
        auto instance = service->get_as<std::string>("instance");
        auto event = service->get_as<std::string>("event");
        config.m_configuredServices.push_back(iox::capro::ServiceDescription(
                                                  iox::capro::IdString(iox::cxx::TruncateToCapacity, *name),
                                                  iox::capro::IdString(iox::cxx::TruncateToCapacity, *instance),
                                                  iox::capro::IdString(iox::cxx::TruncateToCapacity, *event)));
        LogDebug() << "[TomlGatewayConfigParser] Loaded service: {" << *name << ", " << *instance << ", " << *event << "}";
    }

    return iox::cxx::success<GatewayConfig>(config);
}

iox::cxx::expected<iox::dds::TomlGatewayConfigParseError>
iox::dds::TomlGatewayConfigParser::validate(const cpptoml::table& parsedToml) noexcept
{
    // Check for expected fields
    auto serviceArray = parsedToml.get_table_array("services");
    if(!serviceArray)
    {
        LogError() << "[TomlGatewayConfigParser] Incomplete configuration provided.";
        return iox::cxx::error<TomlGatewayConfigParseError>(TomlGatewayConfigParseError::INCOMPLETE_CONFIGURATION);
    }

    uint8_t count = 0;
    for(const auto& service: *serviceArray)
    {
        ++count;
        auto serviceName = service->get_as<std::string>("service");
        auto instance = service->get_as<std::string>("instance");
        auto event = service->get_as<std::string>("event");

        // Check for incomplete service descriptions
        if(!serviceName || !instance || !event)
        {
            LogError() << "[TomlGatewayConfigParser] Incomplete service description at entry: " << count;
            return iox::cxx::error<TomlGatewayConfigParseError>(TomlGatewayConfigParseError::INCOMPLETE_SERVICE_DESCRIPTION);
        }

        // Check for invalid characters in strings
        if(hasInvalidCharacter(*serviceName) ||
           hasInvalidCharacter(*instance) ||
           hasInvalidCharacter(*event))
        {
            LogError() << "[TomlGatewayConfigParser] Invalid service description at entry: " << count;
            return iox::cxx::error<TomlGatewayConfigParseError>(TomlGatewayConfigParseError::INVALID_SERVICE_DESCRIPTION);
        }

    }

    return iox::cxx::success<>();

}

bool iox::dds::TomlGatewayConfigParser::hasInvalidCharacter(std::string s) noexcept
{
    // See: https://design.ros2.org/articles/topic_and_service_names.html
    const std::regex regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    auto isInvalid = !std::regex_match(s, regex);
    if(isInvalid)
    {
        LogError() << "[TomlGatewayConfigParser] Invalid character in name: " + s;
    }
    return isInvalid;
}
