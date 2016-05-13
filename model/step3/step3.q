//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 5535), March 2014

/*
Le temps d'attente pour un Pi\u00e9ton est au plus de 3P (si justeCalled est vrai alors il doit attendre deux feux soit 2P et si un bus est appel\u00e9 avant il faut attendre un feu pour le bus soit 1P) ici on mets 4P parce que il reste jusqu'\u00e0 la fin du 4ieme feu au vert
*/
A[] not( Crosswalk.Green && Crosswalk.ct>4*P )

/*
Le temps d'attente pour un Pi\u00e9ton est au plus de 2P (si justeCalled est vrai alors il doit attendre deux feux soit 2P) ici on mets 3P parce que il reste jusqu'\u00e0 la fin du 3ieme feu au vert
*/
A[] not( Bus.Green && Bus.bt>3*P )

/*
Not crosswalk light green and another green at the same time
*/
A[] not (((Crosswalk.Green or Crosswalk.Free) and  (Bus.Green or Bus.Free)) or ((Crosswalk.Green or Crosswalk.Free) and TrafficLightFrontBack.Green) or ((Crosswalk.Green or Crosswalk.Free) and TrafficLightLeftRight.Green))

/*
Not bus light green and another green at the same time
*/
A[] not (((Bus.Green or Bus.Free) and  (Crosswalk.Green or Crosswalk.Free)) or ((Bus.Green or Bus.Free) and TrafficLightFrontBack.Green) or ((Bus.Green or Bus.Free) and TrafficLightLeftRight.Green))

/*

*/
A[] not deadlock

/*
Le feu redevient vert avant 4P 
*/
A[] not( TrafficLightFrontBack.Green && TrafficLightFrontBack.tlfbc>4*P )

/*
Le feu redevient vert avant 4P 
*/
A[] not( TrafficLightLeftRight.Green && TrafficLightLeftRight.tllrc>4*P )

/*
if crosswalk and bus are red, one of traffic light are green
*/
A[] (not (Crosswalk.Green or Crosswalk.Free) and not( Bus.Green or Bus.Free)) imply  (TrafficLightLeftRight.Green or TrafficLightFrontBack.Green)

/*

*/
Crosswalk.Called --> Crosswalk.Green

/*
Check if all states verify the follow proprety: If the crosswalk is waiting to be green, a trafficlight cannot be green when the other is in Crosswalk mode and is waiting for the crosswalkLight to be green
*/
A[] not ( (TrafficLightFrontBack.Green and TrafficLightLeftRight.Crosswalk and Crosswalk.Waiting) or (TrafficLightLeftRight.Green and TrafficLightFrontBack.Crosswalk and Crosswalk.Waiting) )
