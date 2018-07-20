//
// Created by dongmin on 18. 7. 20.
//

#include <detail/algorithm/triangulate_surface.h>
#include "triangulation.h"

#include "features/Surface.h"
#include "features/Triangle.h"
#include "detail/cgal_config.h"
#include "detail/feature/plane.h"
#include "detail/feature/polygon.h"

using namespace std;

namespace TM2IN {
    namespace algorithm {
        int triangulate(Surface *&pSurface, std::vector<Triangle*>& result) {
            result.clear();

            std::vector<Vertex*> vertexList = pSurface->getVerticesList();
            if (vertexList.size() <= 4){
                pSurface->setNormal(pSurface->getSimpleNormal());
            }

            if (vertexList.size() == 3) {
                Triangle* newTriangle = new Triangle(vertexList);
                newTriangle->sf_id = pSurface->sf_id + "_0";
                result.push_back(newTriangle);
                return 0;
            }
            else
                return TM2IN::detail::algorithm::triangulate_surface(pSurface, result);

        }
    }
}