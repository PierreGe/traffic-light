//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 5648), July 2014

/*

*/
control: A[] not( (TrafficLightLeftRight.Green and TrafficLightFrontBack.Green) or (TrafficLightLeftRight.Green and Crosswalk.Green) or (Crosswalk.Green and TrafficLightFrontBack.Green))
