#include "EmscriptenArchive.h"
#include <fstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace MX;
using namespace Serialization;

//TODO async
void EmscriptenArchive::Load(const LoadCallback& cb)
{
#ifdef __EMSCRIPTEN__
	auto raw_cb = new LoadCallback{cb};
	emscripten_idb_async_load("EmscriptenArchive", path().c_str(), (void*)raw_cb, 
		[](void* arg, void* ptr, int num) 
	{
		std::unique_ptr<LoadCallback> cb( (LoadCallback*) arg );
		(*cb)({(char*)ptr, (std::string::size_type) num}, Result::OK);
	}, 
		[](void* arg) 
	{ 
		std::unique_ptr<LoadCallback> cb( (LoadCallback*) arg );
		(*cb)("", Result::ErrorFatal);
	});
#else
	cb("", Result::ErrorFatal);
#endif
}

void EmscriptenArchive::Save(std::string&& data, const SaveCallback& cb)
{
#ifdef __EMSCRIPTEN__
	auto raw_cb = new SaveCallback{cb};
	emscripten_idb_async_store("EmscriptenArchive", path().c_str(), (void*)data.c_str(), data.size(), (void*)raw_cb, 
		[](void* arg) 
		{
			std::unique_ptr<SaveCallback> cb( (SaveCallback*) arg );
			(*cb)(Result::OK);
		}, 
		[](void* arg) 
		{ 
			std::unique_ptr<SaveCallback> cb( (SaveCallback*) arg );
			(*cb)(Result::ErrorFatal);
		});
#else
	cb(Result::ErrorFatal);
#endif
}