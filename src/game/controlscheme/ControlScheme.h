#ifndef MXCONTROLSCHEME
#define MXCONTROLSCHEME
#include "utils/Utils.h"
#include "utils/shapes/Rectangle.h"
#include "utils/SignalizingVariable.h"
#include "utils/FunctorsQueue.h"
#include <array>
#include "SDL_keycode.h"

namespace MX
{
    namespace Game
    {

        class Action;
		class ActionBase;
        std::shared_ptr<Action> actionForKey(int keycode);
		std::shared_ptr<Action> actionForJoystickButton(unsigned joy, unsigned button);
		std::shared_ptr<Action> actionForJoystickAxis(unsigned joy, unsigned axis, float margin);


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

        class ActionBase : public MX::SignalTrackable
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
			float weight() { return _weight; }

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
                }, this);
            }

			float _weight = 0.0f;
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
                }, this);
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
                    action.onTrigger.static_connect([&, index]()
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

			void bindJoyButton(unsigned index, unsigned joy, int button)
			{
				actions[index].bind(actionForJoystickButton(joy, button));
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
                p->state.onValueChanged.static_connect([&](auto...) { recalcState(); });
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

			void BindJoypad(unsigned joy)
			{
				float m = 0.45f;
				bind(Type{ 0, -1 }, actionForJoystickAxis(joy, 1, -m));
				bind(Type{ 0, 1 }, actionForJoystickAxis(joy, 1, m));
				bind(Type{ -1, 0 }, actionForJoystickAxis(joy, 0, -m));
				bind(Type{ 1, 0 }, actionForJoystickAxis(joy, 0, m));
			}

			void bindKeysWSAD()
			{
				bindKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
			}

			void bindKeysArrows()
			{
				bindKeys(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);
			}
        protected:
            void recalcState()
            {
                Type newState;
				float w = 0.0f;
                for (auto& e : _actions)
                {
                    if (!e._action->state)
                        continue;
					if (_exclusive)
					{
						if (e._action->weight() < w)
							continue;
						w = e._action->weight();
						newState = e._value;
					}
					else
						newState += e._value;
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
				this->target.onValueChanged.static_connect([&](auto &v, auto &o)
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
                this->Call([&]() { onTick((Type)this->target.directValueAccess()); });
            }
        };
        
    };
};


#endif
