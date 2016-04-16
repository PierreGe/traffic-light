//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 4598), October 2010

/*
Check crosswalks are well syncronized with signal ( Never green\/red at the same time than the signal it cross )
*/
A[] not ( (CrossLeftRight.Green and SignFrontBack.Green ) or (CrossLeftRight.Red and SignFrontBack.Red) or (SignLeftRight.Green and CrossFrontBack.Green ) or (SignLeftRight.Red and CrossFrontBack.Red) )

/*
Check that signals are never the same color
*/
A[] not( (SignLeftRight.Green and SignFrontBack.Green) or (SignLeftRight.Red and SignFrontBack.Red)
