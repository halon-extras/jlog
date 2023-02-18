#include <HalonMTA.h>
#include <string>
#include <syslog.h>
#include <jlog.h>
#include <memory>
#include <stdexcept>
#include <map>

struct jlogContext
{
	jlog_ctx* writerContext;
};

std::map<std::string, std::shared_ptr<jlogContext>> jlogContexts;

HALON_EXPORT
int Halon_version()
{
	return HALONMTA_PLUGIN_VERSION;
}

HALON_EXPORT
void jlogger(HalonHSLContext* hhc, HalonHSLArguments* args, HalonHSLValue* ret)
{
	HalonHSLValue* id_ = HalonMTA_hsl_argument_get(args, 0);
	if (!id_ || HalonMTA_hsl_value_type(id_) != HALONMTA_HSL_TYPE_STRING)
		return;
	HalonHSLValue* payload_ = HalonMTA_hsl_argument_get(args, 1);
	if (!payload_ || HalonMTA_hsl_value_type(payload_) != HALONMTA_HSL_TYPE_STRING)
		return;

	char* id = nullptr;
	HalonMTA_hsl_value_get(id_, HALONMTA_HSL_TYPE_STRING, &id, nullptr);
	char* payload = nullptr;
	size_t payloadlen = 0;
	HalonMTA_hsl_value_get(payload_, HALONMTA_HSL_TYPE_STRING, &payload, &payloadlen);

	auto x = jlogContexts.find(id);
	if (x == jlogContexts.end())
		return;

	bool r = false;
	if (jlog_ctx_write(x->second->writerContext, payload, payloadlen) != 0)
		syslog(LOG_CRIT, "jlog: jlog_ctx_write failed: %d %s", jlog_ctx_err(x->second->writerContext), jlog_ctx_err_string(x->second->writerContext));
	else
		r = true;

	HalonMTA_hsl_value_set(ret, HALONMTA_HSL_TYPE_BOOLEAN, &r, 0);
	return;
}

HALON_EXPORT
bool Halon_init(HalonInitContext* hic)
{
	HalonConfig* cfg = nullptr;
	HalonMTA_init_getinfo(hic, HALONMTA_INIT_CONFIG, nullptr, 0, &cfg, nullptr);
	if (!cfg)
		return false;

	try {
		auto queues = HalonMTA_config_object_get(cfg, "queues");
		if (queues)
		{
			size_t l = 0;
			HalonConfig* queue;
			while ((queue = HalonMTA_config_array_get(queues, l++)))
			{
				auto x = std::make_shared<jlogContext>();

				const char* id = HalonMTA_config_string_get(HalonMTA_config_object_get(queue, "id"), nullptr);
				const char* path = HalonMTA_config_string_get(HalonMTA_config_object_get(queue, "path"), nullptr);

				if (!id || !path)
					throw std::runtime_error("missing required property");

				jlog_ctx* ctx = jlog_new(path);
				if (jlog_ctx_init(ctx) != 0)
				{
					if (jlog_ctx_err(ctx) != JLOG_ERR_CREATE_EXISTS)
						throw std::runtime_error(jlog_ctx_err_string(ctx));

					HalonConfig* subscribers = HalonMTA_config_object_get(queue, "subscribers");
					if (subscribers)
					{
						size_t h = 0;
						HalonConfig* subscriber;
						while ((subscriber = HalonMTA_config_array_get(subscribers, h++)))
							jlog_ctx_add_subscriber(ctx, HalonMTA_config_string_get(subscriber, nullptr), JLOG_BEGIN);
					}
				}
				jlog_ctx_close(ctx);
				ctx = jlog_new(path);
				if (jlog_ctx_open_writer(ctx) != 0)
					throw std::runtime_error(jlog_ctx_err_string(ctx));
				x->writerContext = ctx;

				jlogContexts[id] = x;
			}
		}
	} catch (const std::runtime_error& e) {
		syslog(LOG_CRIT, "jlog: %s", e.what());
		return false;
	}

	return true;
}

HALON_EXPORT
void Halon_cleanup()
{
	for (auto & i : jlogContexts)
	{
		jlog_ctx_close(i.second->writerContext);
	}
}

HALON_EXPORT
bool Halon_hsl_register(HalonHSLRegisterContext* ptr)
{
	HalonMTA_hsl_module_register_function(ptr, "jlogger", &jlogger);
	return true;
}
