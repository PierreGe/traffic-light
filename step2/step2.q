//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 4598), October 2010

/*

*/
control : A[] not( Crosswalk.Free && pulsed==true)

/*

*/
control : A[] not( Crosswalk.Waiting && pulsed==true)

/*

*/
control: A<> TrafficLightLeftRight.Green

/*
Check if all states verify the follow proprety: If the crosswalk is waiting to be green, a trafficlight cannot be green when the other is in Crosswalk mode and is waiting for the crosswalkLight to be green
*/
control: A[] not ( (TrafficLightFrontBack.Green and TrafficLightLeftRight.Crosswalk and Crosswalk.Waiting) or (TrafficLightLeftRight.Green and TrafficLightFrontBack.Crosswalk and Crosswalk.Waiting) )

/*
Check if all states verify the proprety : The two traffic lights cannot be green at the same time or the crosswalk and one of the traffic light cannot be green too
*/
control : A[] not( (TrafficLightLeftRight.Green and TrafficLightFrontBack.Green) or (TrafficLightLeftRight.Green and Crosswalk.Green) or (Crosswalk.Green and TrafficLightFrontBack.Green))
