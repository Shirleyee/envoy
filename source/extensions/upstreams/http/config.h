#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <string>

#include "envoy/config/core/v3/extension.pb.h"
#include "envoy/extensions/upstreams/http/v3/http_protocol_options.pb.h"
#include "envoy/extensions/upstreams/http/v3/http_protocol_options.pb.validate.h"
#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"
#include "envoy/server/transport_socket_config.h"

#include "common/common/logger.h"
#include "common/protobuf/message_validator_impl.h"

namespace Envoy {
namespace Extensions {
namespace Upstreams {
namespace Http {

class ProtocolOptionsConfigImpl : public Upstream::ProtocolOptionsConfig {
public:
  ProtocolOptionsConfigImpl(
      const envoy::extensions::upstreams::http::v3::HttpProtocolOptions& options);
  // Constructor for legacy (deprecated) config.
  ProtocolOptionsConfigImpl(
      const envoy::config::core::v3::Http1ProtocolOptions& http1_settings,
      const envoy::config::core::v3::Http2ProtocolOptions& http2_options,
      const envoy::config::core::v3::HttpProtocolOptions& common_options,
      const absl::optional<envoy::config::core::v3::UpstreamHttpProtocolOptions> upstream_options,
      bool use_downstream_protocol, bool use_http2);

  const Envoy::Http::Http1Settings http1_settings_;
  const envoy::config::core::v3::Http2ProtocolOptions http2_options_;
  const envoy::config::core::v3::HttpProtocolOptions common_http_protocol_options_;
  const absl::optional<envoy::config::core::v3::UpstreamHttpProtocolOptions>
      upstream_http_protocol_options_;

  bool use_downstream_protocol_{};
  bool use_http2_{};
  bool use_alpn_{};
};

class ProtocolOptionsConfigFactory : public Server::Configuration::ProtocolOptionsFactory {
public:
  Upstream::ProtocolOptionsConfigConstSharedPtr createProtocolOptionsConfig(
      const Protobuf::Message& config,
      Server::Configuration::ProtocolOptionsFactoryContext& context) override {
    const auto& typed_config = MessageUtil::downcastAndValidate<
        const envoy::extensions::upstreams::http::v3::HttpProtocolOptions&>(
        config, context.messageValidationVisitor());
    return std::make_shared<ProtocolOptionsConfigImpl>(typed_config);
  }
  std::string category() const override { return "envoy.upstream_options"; }
  std::string name() const override {
    return "envoy.extensions.upstreams.http.v3.HttpProtocolOptions";
  }
  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return std::make_unique<envoy::extensions::upstreams::http::v3::HttpProtocolOptions>();
  }
  ProtobufTypes::MessagePtr createEmptyProtocolOptionsProto() override {
    return std::make_unique<envoy::extensions::upstreams::http::v3::HttpProtocolOptions>();
  }
};

DECLARE_FACTORY(ProtocolOptionsConfigFactory);

} // namespace Http
} // namespace Upstreams
} // namespace Extensions
} // namespace Envoy
