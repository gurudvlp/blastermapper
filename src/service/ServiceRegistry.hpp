#ifndef HEADER_SERVICE_REGISTRY
#define HEADER_SERVICE_REGISTRY

#include <memory>
#include <string>
#include <unordered_map>
namespace blastmap {

class IService {
public:
    virtual ~IService() = default;
    virtual std::string serviceName() const = 0;
    virtual bool tick() { return true; }
};

class ServiceRegistry {
public:
    void registerService(std::unique_ptr<IService> service)
    {
        if(!service) { return; }
        services_.emplace(service->serviceName(), std::move(service));
    }

    template<typename T>
    T *getServiceAs(const std::string &serviceName)
    {
        auto it = services_.find(serviceName);
        if(it == services_.end()) { return nullptr; }
        return dynamic_cast<T *>(it->second.get());
    }

    template<typename Callback>
    void forEachService(Callback &&callback)
    {
        for(auto &entry : services_)
        {
            if(!callback(*entry.second)) { break; }
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IService>> services_;
};

} // namespace blastmap

#endif
