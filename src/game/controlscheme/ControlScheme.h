#ifndef MXCONTROLSCHEME
#define MXCONTROLSCHEME
#include "Utils/Utils.h"
#include "Utils/shapes/Rectangle.h"
#include "Utils/SignalizingVariable.h"
#include "Utils/FunctorsQueue.h"
#include <array>

namespace MX
{
    namespace Game
    {

        class Action;
		class ActionBase;
        std::shared_ptr<Action> actionForKey(int keycode);


        class ControlScheme
        {
            friend class ActionBase;
        public:
            ControlScheme()
            {

            }

            void Run();
            auto& queue() { return _queue; }
        protected:
            void AddAction(ActionBase* action)
            {
                _actions.push_back(action);
            }

            MX::FunctorsQueue _queue;
            std::vector<ActionBase*> _actions;
        };

        class ActionBase
        {
        public:
            ActionBase(ControlScheme* scheme) : _scheme(scheme) 
            {
                if (scheme)
                    scheme->AddAction(this);
            }
            virtual ~ActionBase() {}

            virtual void Run()
            {

            }

            void SetScheme(ControlScheme* scheme)
            {
                if (_scheme)
                    return;
                _scheme = scheme;
                if (scheme)
                    scheme->AddAction(this);
            }
        protected:

            template<typename T>
            void Call(const T& t)
            {
                if (!_scheme)
                {
                    t();
                    return;
                }
                _scheme->queue().queueFunctor([=]()
                {
                    t();
                });
            }

            ControlScheme* _scheme;
        };

        template<typename T>
        class ActionProxy
        {
        public:
            using pointer = std::shared_ptr<T>;

            void bind(const pointer& proxy) { _children.push_back(proxy);  onAddedChild(proxy); }

            void bindKey(int keycode)
            {
                bind(actionForKey(keycode));
            }
        protected:
            virtual void onAddedChild(const pointer& proxy) {}
            std::vector<pointer> _children;
        };

        class Action : public ActionBase, public ActionProxy<Action>
        {
        public:
            using ActionBase::ActionBase;
            using Callback = std::function<void(void)>;

            MX::Signal<void(void)> onTrigger;

            SignalizingVariable<bool> state = false;

            void whileOn(const Callback& c)
            {
                _whileOn.push_back(c);
            }
        protected:
            void Run() override
            {
                ActionBase::Run();

                if (!state)
                    return;
                for (auto&f : _whileOn)
                    f();
            }

            void SetState(bool _state)
            {
                if (state == _state)
                    return;
                Call([&, _state]()
                {
                    state = _state;
                    if (state)
                        onTrigger();
                });
            }

            void onAddedChild(const pointer& proxy) override
            {
                proxy->state.onValueChanged.connect([&](bool v, bool old) 
                {
                    SetState(v);
                });
            }

            std::vector<Callback> _whileOn;
        };

        
        class TickingHelper
        {
        public:
            TickingHelper(ControlScheme* scheme, float tickRate = 0.0f)
            {
                _scheme = scheme;
                _tickRate = tickRate;
            }

            void SetTickRate(float tickRate)
            {
                _tickRate = tickRate;
            }

            void SetFirstTickMultiplier(float multiplier)
            {
                _firstTickMultiplier = multiplier;
            }
        protected:
            void StartTicking()
            {
                if (_ticking)
                    return;
                _conn = std::make_shared<int>();
                _ticking = true;
                OnTimeout(_tickRate * _firstTickMultiplier);
            }

            void StopTicking()
            {
                if (!_ticking)
                    return;
                _conn = nullptr;
                _ticking = false;
            }

            virtual void onGotTick() {}

            void OnTimeout(float tick)
            {
                onGotTick();

                if (tick && _scheme)
                    _scheme->queue().planWeakFunctor(tick, [&] { OnTimeout(_tickRate); }, _conn);
            }
            float _firstTickMultiplier = 1.0f;
            float _tickRate = 0.0f;
            bool _ticking = false;
            std::shared_ptr<int> _conn;
            ControlScheme* _scheme;
        };

        class TickingAction : public Action, public TickingHelper
        {
        public:
            TickingAction(ControlScheme* scheme, float tickRate = 0.0f) : Action(scheme), TickingHelper(scheme, tickRate)
            {
                state.onValueChanged.connect([&](bool v, bool o) 
                {
                    if (v)
                        StartTicking();
                    else
                        StopTicking();
                });
            }

            MX::Signal<void(void)> onTick;

        protected:
            void onGotTick() override 
            {
                ActionBase::Call([&]() { onTick(); });
            }            
        };

        template<typename T, unsigned size>
        class ActionList : public ActionBase
        {
        public:
            ActionList(ControlScheme* scheme) : ActionBase(scheme)
            {
                unsigned index = 0;
                for (auto& action : actions)
                {
                    action.SetScheme(scheme);
                    action.onTrigger.connect([&, index]()
                    {
                        onTrigger(index);
                    });
                    index++;
                }
            }

            MX::Signal<void(int)> onTrigger;

            std::vector<T> actions { size, nullptr };

            bool anyIsOn()
            {
                return std::any_of(actions.begin(), actions.end(), [](T &item) { return item.state; });
            }

            void bind(unsigned index, const std::shared_ptr<T>& proxy)
            {
                actions[index].bind(proxy);
            }

            void bindKey(unsigned index, int keycode)
            {
                actions[index].bind(actionForKey(keycode));
            }

            void whileOn(const std::function<Action::Callback(unsigned)>& c)
            {
                for (unsigned i = 0; i < actions.size(); i++)
                    actions[i].whileOn(c(i));
            }
        };

        class Target : public ActionBase
        {
        public:
            using ActionBase::ActionBase;

            SignalizingVariable<glm::vec2> target;
        };

        template<typename Type = glm::vec2>
        class TargetDirection : public ActionBase
        {
            struct Data
            {
                Type            _value;
                Action::pointer _action;
            };
        public:
            using ActionBase::ActionBase;
            
            SignalizingVariable<Type> target;

            void bind(const Type& v, const Action::pointer& p)
            {
                _actions.push_back(Data{ v, p });
                p->state.onValueChanged.connect([&](auto...) { recalcState(); });
            }

            void bindKey(const Type& v, int keycode)
            {
                auto key = actionForKey(keycode);
                bind(v, key);
            }

            void bindKeys(int up, int down, int left, int right)
            {
                bindKey(Type{ 0, -1 }, up);
                bindKey(Type{ 0, 1 },  down);
                bindKey(Type{ -1, 0 }, left);
                bindKey(Type{ 1, 0 }, right);
            }
        protected:
            void recalcState()
            {
                Type newState;
                for (auto& e : _actions)
                {
                    if (!e._action->state)
                        continue;
                    newState += e._value;
                    if (_exclusive)
                        break;
                }
                if (newState == target.directValueAccess())
                    return;
                Call([&, newState]()
                {
                    target = newState;
                });
            }

            bool              _exclusive = true;
            std::vector<Data> _actions;
        };

        template<typename Type = glm::vec2>
        class TickingTargetDirection : public TargetDirection<Type>, public TickingHelper
        {
        public:
            TickingTargetDirection(ControlScheme* scheme, float tickRate = 0.0f) : TargetDirection<Type>(scheme), TickingHelper(scheme, tickRate)
            {
				Target::target.onValueChanged.connect([&](auto &v, auto &o)
                {
                    bool isZero = v == Type{ 0,0 };
                    bool wasZero = o == Type{ 0,0 };
                    if (isZero == wasZero)
                        return;

                    if (!isZero)
                        StartTicking();
                    else
                        StopTicking();
                });
            }

            MX::Signal<void(const Type&)> onTick;
        protected:
            void onGotTick() override
            {
                Call([&]() { onTick(Target::target); });
            }
        };
        
    };
};


#endif