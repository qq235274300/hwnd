#pragma once
#include "Bindable.h"
#include <type_traits>
#include <memory>
#include <unordered_map>
class Codex
{
public:
	template<class T, typename...Params>
	static std::shared_ptr<T> Resolve(Graphics& gfx,Params&&...p)noxnd
	{
		static_assert(std::is_base_of<Bind::Bindable, T>::value, "Can only resolve classes derived from Bindable");
		return Get()._Resolve<T>(gfx,std::forward<Params>(p)...);
	}
private:
	template<class T,typename...Params>
	std::shared_ptr<T> _Resolve(Graphics& gfx,Params&&...p) noxnd
	{
		const auto uid = T::GenerateUID(std::forward<Params>(p)...);
		auto i = binds.find(uid);
		if ( i == binds.end())
		{
			auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
			binds[uid] = bind;
			return bind;
		}
		else
		{
			return std::static_pointer_cast<T>(i->second);
		}	
	}

	static Codex& Get()
	{
		static Codex ex;
		return ex;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Bind::Bindable>> binds;
};