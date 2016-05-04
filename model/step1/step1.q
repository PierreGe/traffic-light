//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 4598), October 2010

/*

*/
control_t*(Controler.P,0): A<> TrafficLightLeftRight.Red and TrafficLightFrontBack.Green

/*

*/
control_t*(Controler.P,0): A<> TrafficLightLeftRight.Green and TrafficLightFrontBack.Red

/*

*/
E[] not( TrafficLightLeftRight.Green and TrafficLightFrontBack.Green )
