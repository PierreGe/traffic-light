//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 4598), October 2010

/*

*/
control_t*(4*P,0): A<> TrafficLightLeftRight.Red and TrafficLightFrontBack.Green and Crosswalk.Red

/*

*/
control_t*(4*P,0): A<> TrafficLightLeftRight.Green and TrafficLightFrontBack.Red and Crosswalk.Red

/*

*/
A[] Crosswalk.Free imply pulsed==false

/*

*/
A[] Crosswalk.Waiting imply pulsed==false

/*
Check if all states verify the follow proprety: If the crosswalk is waiting to be green, a trafficlight cannot be green when the other is in Crosswalk mode and is waiting for the crosswalkLight to be green
*/
A[] not ( (TrafficLightFrontBack.Green and TrafficLightLeftRight.Crosswalk and Crosswalk.Waiting) or (TrafficLightLeftRight.Green and TrafficLightFrontBack.Crosswalk and Crosswalk.Waiting) )

/*
Check if all states verify the proprety : The two traffic lights cannot be green at the same time or the crosswalk and one of the traffic light cannot be green too
*/
A[] not( (TrafficLightLeftRight.Green and TrafficLightFrontBack.Green) or (TrafficLightLeftRight.Green and Crosswalk.Green) or (Crosswalk.Green and TrafficLightFrontBack.Green))
