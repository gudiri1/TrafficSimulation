#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });

    T phase = std::move(_queue.back());
    _queue.pop_back();

    return phase;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // perform modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // add element to queue
    //std::cout << "    #" << v.getID() << " will be added to the queue" << std::endl;
    _queue.push_back(std::move(msg));
    _cond.notify_one(); // notify client after pushing new Vehicle into vector
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.

    while(true)
    {
        auto phase = _Mqueue.receive();

        if (phase == green)
        {
            break;
        }
        else
        {
            
            
        }
        
    }
}


TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, 
    //use the thread queue in the base class. 


    cycleThroughPhases();

}



// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

// initalize variables
    // bool hasEnteredIntersection = false;
    // double cycleDuration = 4000; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();

        srand((unsigned) time(0));
        double cycleDuration = ( 4 + (rand() % 3) ) * 1000;

        if (timeSinceLastUpdate >= cycleDuration)
        {
            
            if (_currentPhase == red)
            {
                _currentPhase = green;
            }
            else
            {
                _currentPhase = red;
            }

        /*  std::vector<std::future<void>> futures;
        // create a new Vehicle instance and move it into the queue
        int v = i;
        futures.emplace_back(std::async(std::launch::async, &MessageQueue<int>::send, queue, std::move(v)));   */

        _Mqueue.send(std::move(_currentPhase));
                
            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }
    } // eof simulation loop

    
}
