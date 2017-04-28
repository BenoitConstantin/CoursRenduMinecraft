#pragma once
#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"

class My_Physics
{
public :
	static bool SegmentPlanIntersect(NYVert3Df segPoint1, NYVert3Df segPoint2, NYVert3Df vecteurPlan, NYVert3Df pointPlan, NYVert3Df* intersectPos)
	{
		NYVert3Df dirSegment = segPoint2 - segPoint1;

		float denom = (dirSegment.scalProd(vecteurPlan));

		if (denom == 0)
			return false;

		float t = (pointPlan - segPoint1).scalProd(vecteurPlan) / denom;

		*intersectPos = segPoint1 + dirSegment*t;

		return ((((*intersectPos) - segPoint1).scalProd((*intersectPos) - segPoint2)) < 0);
	}

	/*static bool IntersectCube(NYVert3Df segPoint1, NYVert3Df segPoint2,NYVert3Df cubePos,  NYCube* cube, NYVert3Df* intersect)
	{
		if (cube == NULL)
			return false;


		///Une face ....
		NYVert3Df faceNorm = NYVert3Df(cube->CUBE_SIZE, 0 , 0).vecProd( NYVert3Df(0,0, cube->CUBE_SIZE));

		if (SegmentPlanIntersect(segPoint1, segPoint2, faceNorm, cubePos, intersect))
		{
			if ()
				return true;
		}

		return false;
	}*/

	/**
	* Attention ce code n'est pas optimal, il est compréhensible. Il existe de nombreuses
	* versions optimisées de ce calcul. Il faut donner les points dans l'ordre (CW ou CCW)
	*/
	static inline bool intersecDroiteCubeFace(NYVert3Df & debSegment, NYVert3Df & finSegment,
		NYVert3Df & p1, NYVert3Df & p2, NYVert3Df & p3, NYVert3Df & p4,
		NYVert3Df & inter)
	{
		//On calcule l'intersection
		bool res = SegmentPlanIntersect(debSegment, finSegment, (p1-p2).vecProd(p1-p3), p2, &inter);

		if (!res)
			return false;

		//On fait les produits vectoriels
		NYVert3Df v1 = p2 - p1;
		NYVert3Df v2 = p3 - p2;
		NYVert3Df v3 = p4 - p3;
		NYVert3Df v4 = p1 - p4;

		NYVert3Df n1 = v1.vecProd(inter - p1);
		NYVert3Df n2 = v2.vecProd(inter - p2);
		NYVert3Df n3 = v3.vecProd(inter - p3);
		NYVert3Df n4 = v4.vecProd(inter - p4);

		//on compare le signe des produits scalaires
		float ps1 = n1.scalProd(n2);
		float ps2 = n2.scalProd(n3);
		float ps3 = n3.scalProd(n4);

		if (ps1 >= 0 && ps2 >= 0 && ps3 >= 0)
			return true;

		return false;


	}

	static int nysign(float value)
	{
		if (value < 0)
			return -1;
		else if (value > 0)
			return 1;
		else
			return 0;
	}
};