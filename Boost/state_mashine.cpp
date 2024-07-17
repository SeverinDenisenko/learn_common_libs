// Copyright 2024 Severin Denisenko

#include <iostream>

#include <boost/msm/back11/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

template <typename T>
void print(const T& t)
{
    std::cout << t << std::endl;
}

struct turn_on {
    bool hot;
};
struct turn_off { };
struct burn_out { };

// front-end: define the FSM structure
struct lamp_ : public msm::front::state_machine_def<lamp_> {

    template <class Event, class FSM>
    void on_entry(Event const&, FSM&)
    {
        print("entering: Lamp");
    }

    template <class Event, class FSM>
    void on_exit(Event const&, FSM&)
    {
        print("leaving: Lamp");
    }

    struct On : public msm::front::state<> {
        using deferred_events = mpl::vector<turn_on>;

        template <class Event, class FSM>
        void on_entry(Event const&, FSM&)
        {
            print("entering: On");
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            print("leaving: On");
        }
    };

    struct Off : public msm::front::state<> {
        using deferred_events = mpl::vector<turn_off, burn_out>;

        template <class Event, class FSM>
        void on_entry(Event const&, FSM&)
        {
            print("entering: Off");
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            print("leaving: Off");
        }
    };

    struct Burned : public msm::front::state<> {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&)
        {
            print("entering: Burned");
        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            print("leaving: Burned");
        }
    };

    // the initial state of the player SM. Must be defined
    using initial_state = mpl::vector<Off>;

    // transition actions
    void turned_on(turn_on const&)
    {
        print("lamp::turned_on");
    }
    void turned_off(turn_off const&)
    {
        print("lamp::turned_off");
    }
    void burned(burn_out const&)
    {
        print("lamp::burned");
    }

    // guard conditions

    bool can_turn_on(turn_on const& ev)
    {
        return ev.hot;
    }

    bool always(auto const& ev)
    {
        return true;
    }

    using l = lamp_;

    // Transition table
    // clang-format off
    struct transition_table : mpl::vector<
        row<Off,  turn_on,   On,      &l::turned_on,   &l::can_turn_on>,
        row<On,   turn_off,  Off,     &l::turned_off,  &l::always>,
        row<On,   burn_out,  Burned,  &l::burned,      &l::always>>
    { };
    // clang-format on

    // Replaces the default no-transition response.
    template <class FSM, class Event>
    void no_transition(Event const& e, FSM&, int state)
    {
        print("No transition");
    }
};

// Pick a back-end
using lamp = msm::back11::state_machine<lamp_>;

int main()
{
    { // Simple transitions
        lamp p;
        p.start();

        p.process_event(turn_on { .hot = true });
        p.process_event(turn_off());

        p.process_event(turn_on());
        p.process_event(burn_out());

        p.stop();
    }
    { // Deffered events
        lamp p;
        p.start();

        p.process_event(turn_on { .hot = true });

        p.process_event(turn_on());

        p.stop();
    }
    return 0;
}
