#pragma once
#include <unordered_map>
#include "utils/Utils.h"
#include <codecvt>
#include <vector>
#include <functional>
#include <map>

namespace MX{

class ScriptParser;
class ScriptObject;

namespace Scriptable
{
	class Value;

	enum class ValueType
	{
		Normal,
		Percent
	};

	namespace Detail
	{
		class ValueMember
		{
			public:
				using Vector = std::vector<std::shared_ptr<Value>>;
				using Map = std::map<std::string, std::shared_ptr<Value>>;

				ValueMember(){}
				~ValueMember(){}

				static bool isOnMainThread;
				virtual void finalizeOnMainThread() {};


				virtual float valueFloat() { return 0.0f; }
				virtual std::string valueString();
				virtual std::wstring valueWstring();

				virtual std::shared_ptr<Value> self() { return nullptr; }
				virtual std::shared_ptr<Value> at(size_t index) { return nullptr; }
				virtual std::shared_ptr<ScriptObject> object() { return nullptr; }
				virtual std::shared_ptr<ScriptObject> createObject() { return nullptr; }
				virtual ScriptObject* createRawObject() { return nullptr; }

				virtual bool isTrue() { return false; }
				virtual bool isConstant() const { return true; }

				virtual size_t size() const
				{
					return 0;
				}

				virtual const Vector& array() const;
				virtual const Map& map() const;

				virtual ValueType type() { return ValueType::Normal; }
		};
				
	}

	class Value : public shared_ptr_init<Value>
	{
	public:
		using Vector = Detail::ValueMember::Vector;
		using Map = Detail::ValueMember::Map;


		Value(const std::string &fullPath);
		Value(const Value&) = delete;
		virtual ~Value(){}
		
		void UpdateObject(const Map& map = {});
		void Update(bool value);
		void Update(float number, bool percent = false);
		void Update(const std::wstring& string);
		void Update(const Vector& members);
		void Update(const std::shared_ptr<ScriptObject>& object);
		void Update(const std::function<float()> &number_functor);

	public: //base



		std::shared_ptr<Value> pointer_to_next() 
		{ 
			if (_member)
				return _member->self();
			return nullptr;
		}

        const std::string& fullPath () const
		{
			return _fullPath;
		}

		operator const std::wstring () const
		{
			return text();
		}

		operator std::string () const
		{
			return atext();
		}



        //this returns static object(that was aready constructed)!
		template< typename T>
		const T& object() const
		{
			return *(std::static_pointer_cast<T>(_member->object()));
		}

		template< typename T>
		std::shared_ptr<T> object_pointer() const
		{
			return std::dynamic_pointer_cast<T>(_member->object());
		}
        
        //this creates a new object!
        template< typename T>
        std::shared_ptr<T> to_object() const
        {
            return std::dynamic_pointer_cast<T>(_member->createObject());
        }

		//this creates a new object!
		template< typename T>
		std::shared_ptr<T> to_raw_object() const
		{
			return std::dynamic_cast<T*>(_member->createRawObject());
		}
//virtuals
		auto type() { return _member->type(); }

		const std::wstring text() const 
		{
			return _member->valueWstring();
		}
        
		const std::string atext() const
		{
			return _member->valueString();
		}

		operator const float () const
		{
			return _member->valueFloat();
		}  


		const bool isTrue() const
		{
			return _member->isTrue();
		}    


		const Value& at(size_t index) const
		{
			return *(_member->at(index));
		}


		const Value& operator [] (size_t index) const
		{
			return *(_member->at(index));
		}


		size_t size() const
		{
			return _member->size();
		}

		const Vector& array() const
		{
			return _member->array();
		}

		const Map& map() const
		{
			return _member->map();
		}
        
		std::unique_ptr<Detail::ValueMember>& member() { return _member; }

		std::string path();
	protected:
        std::string _fullPath;
		std::unique_ptr<Detail::ValueMember> _member;
	};






}



}