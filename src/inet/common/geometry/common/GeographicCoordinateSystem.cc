//
// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/common/geometry/common/GeographicCoordinateSystem.h"

#if defined(WITH_OSGEARTH) && defined(WITH_VISUALIZERS)
#include <osgEarthUtil/ObjectLocator>
#endif

namespace inet {

const GeoCoord GeoCoord::NIL = GeoCoord(degree(NaN), degree(NaN), NaN);

Define_Module(SimpleGeographicCoordinateSystem);

void SimpleGeographicCoordinateSystem::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        playgroundLatitude = degree(par("playgroundLatitude"));
        playgroundLongitude = degree(par("playgroundLongitude"));
        playgroundAltitude = par("playgroundAltitude");
    }
}

Coord SimpleGeographicCoordinateSystem::computePlaygroundCoordinate(const GeoCoord& geographicCoordinate) const
{
    double playgroundX = degree(geographicCoordinate.longitude - playgroundLongitude).get() * cos(fabs(rad(playgroundLatitude).get())) * metersPerDegree;
    double playgroundY = degree(playgroundLatitude - geographicCoordinate.latitude).get() * metersPerDegree;
    return Coord(playgroundX, playgroundY, geographicCoordinate.altitude + playgroundAltitude);
}

GeoCoord SimpleGeographicCoordinateSystem::computeGeographicCoordinate(const Coord& playgroundCoordinate) const
{
    auto geograpicLatitude = playgroundLatitude - degree(playgroundCoordinate.y / metersPerDegree);
    auto geograpicLongitude = playgroundLongitude + degree(playgroundCoordinate.x / metersPerDegree / cos(fabs(rad(playgroundLatitude).get())));
    return GeoCoord(geograpicLatitude, geograpicLongitude, playgroundCoordinate.z - playgroundAltitude);
}

#if defined(WITH_OSGEARTH) && defined(WITH_VISUALIZERS)

Define_Module(OsgGeographicCoordinateSystem);

void OsgGeographicCoordinateSystem::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        auto mapScene = getParentModule()->getOsgCanvas()->getScene();
        mapNode = osgEarth::MapNode::findMapNode(mapScene);
        if (mapNode == nullptr)
            throw cRuntimeError("Count not find map node in the scene");
        auto playgroundLatitude = degree(par("playgroundLatitude"));
        auto playgroundLongitude = degree(par("playgroundLongitude"));
        double playgroundAltitude = par("playgroundAltitude");
        auto playgroundHeading = degree(par("playgroundHeading"));
        auto playgroundElevation = degree(par("playgroundElevation"));
        auto playgroundBank = degree(par("playgroundBank"));
        playgroundPosition = GeoCoord(playgroundLatitude, playgroundLongitude, playgroundAltitude);
        playgroundOrientation = EulerAngles(playgroundHeading, playgroundElevation, playgroundBank);
        auto locatorNode = new osgEarth::Util::ObjectLocatorNode(mapNode->getMap());
        locatorNode->getLocator()->setPosition(osg::Vec3d(degree(playgroundLongitude).get(), degree(playgroundLatitude).get(), playgroundAltitude));
        locatorNode->getLocator()->setOrientation(osg::Vec3d(rad(playgroundHeading).get(), rad(playgroundElevation).get(), rad(playgroundBank).get()));
        locatorNode->getLocator()->getLocatorMatrix(locatorMatrix);
        inverseLocatorMatrix.invert(locatorMatrix);
        delete locatorNode;
    }
}

Coord OsgGeographicCoordinateSystem::computePlaygroundCoordinate(const GeoCoord& geographicCoordinate) const
{
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d ecefCoordinate;
    osg::Vec3d osgGeographicCoordinate(degree(geographicCoordinate.longitude).get(), degree(geographicCoordinate.latitude).get(), geographicCoordinate.altitude);
    mapSrs->getGeographicSRS()->transform(osgGeographicCoordinate, mapSrs->getECEF(), ecefCoordinate);
    auto playgroundCoordinate = osg::Vec4d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z(), 1.0) * inverseLocatorMatrix;
    return Coord(playgroundCoordinate.x(), playgroundCoordinate.y(), playgroundCoordinate.z());
}

GeoCoord OsgGeographicCoordinateSystem::computeGeographicCoordinate(const Coord& playgroundCoordinate) const
{
    auto ecefCoordinate = osg::Vec4d(playgroundCoordinate.x, playgroundCoordinate.y, playgroundCoordinate.z, 1.0) * locatorMatrix;
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d geographicCoordinate;
    mapSrs->getECEF()->transform(osg::Vec3d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z()), mapSrs->getGeographicSRS(), geographicCoordinate);
    return GeoCoord(degree(geographicCoordinate.y()), degree(geographicCoordinate.x()), geographicCoordinate.z());
}

#endif // WITH_OSGEARTH

} // namespace inet

