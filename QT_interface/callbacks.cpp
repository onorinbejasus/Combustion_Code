//#include "enums.hh"

//// tensor files
//#include <time.h>
//#include <fstream>



//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#include <iostream>


//// =======================
//// = Keyboard Operations =
//// =======================

//// The keyboard callback
//void keyboard(unsigned char key, int x, int y)
//{
//   switch (key) {
//		case 'e':				// explore mode
//		case 'E':				// move, rotate, zoom
//			printf("Entering EXPLORE_MODE.\n") ;
//			CurrIM = ExploreMode ;
//			break;
//		case 's': // select mode
//		case 'S':
//			printf("Entering SELECT_MODE.\n") ;
//			CurrIM = SelectMode ;
//			break ;
//		case '1':														// enable/disable volume rendering
//			if (volRenderer.bVisible)
//				volRenderer.bVisible = false ;
//			else
//				volRenderer.bVisible = true ;
//			glutPostRedisplay() ;
//			break ;
//		case '2':													// enable/disable vector field view
//			if( vectorField.bVisible )
//				vectorField.bVisible = false ;
//			else
//				vectorField.bVisible = true ;
//			glutPostRedisplay() ;
//			break ;
//		case '3':													// enable/disable streamlines
//			if(streamlines.bVisible)
//				streamlines.bVisible  = false ;
//			else
//				streamlines.bVisible  = true ;
//			glutPostRedisplay() ;
//			break ;
//		case 'x':  // toggle grid x
//		case 'X':
//			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
//				iSeedpointMoveDir = 0 ;
//			}else{
//				vectorField.ToggleSliceVisibilityX();
//			}
//			glutPostRedisplay();
//			break ;
//		case 'y':	// toggle grid y
//		case 'Y':
//			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
//				iSeedpointMoveDir = 1 ;
//			}else{
//				vectorField.ToggleSliceVisibilityY();
//			}
//			glutPostRedisplay();
//			break ;
//		case 'z':		// toggle grid z
//		case 'Z':
//			if( CurrIM == SelectMode && ObjectCatagory == SEEDPOINT ){
//				iSeedpointMoveDir = 2 ;
//			}
			
//			else{
//				vectorField.ToggleSliceVisibilityZ();
//			}
			
//			glutPostRedisplay();
//			break ;
//		case 'm': 		// enable mag glass
//		case 'M':
//			magnifying = !magnifying;
//			glutPostRedisplay();
//			break;
//		case 'l':		// increase radius
//		case 'L':
//			radius += 0.01;
//			glutPostRedisplay();
//			break;
//		case 'k':			// decrease radius
//		case 'K':
//			radius -= 0.01;
//			glutPostRedisplay();
//			break;
//		case '+':  			// zoom in
//			if(magnifying){
//				zoom += 0.5;
//			}
//			glutPostRedisplay() ;
//			break ;
//		case '-':	// zoom out
			
//			if(magnifying){
//				zoom -= 0.5;
//				if(zoom < 1.0)
//					zoom = 1.0;
//			}
//			glutPostRedisplay() ;
//			break ;
		
//		case '0':
//			if(shift){
//				vectorField.IncreaseSliceNumber();
//				glutPostRedisplay();
//			}
//		break;
//		case '9':
//			if(shift){
//				vectorField.DecreaseSliceNumber();
//			}
//		glutPostRedisplay();
//		break;
		
//		case 27:
//			exit(0);
//			break ;
			
//		case 't':
//		case 'T':
//			shift = !shift;
//			glutPostRedisplay();
//			break;
//   }

//}

//// ====================
//// = Mouse Operations =
//// ====================





