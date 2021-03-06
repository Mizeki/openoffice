/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"

#include "PositionAndSizeHelper.hxx"
#include "macros.hxx"
#include "ChartModelHelper.hxx"
#include "ControllerLockGuard.hxx"
#include <com/sun/star/chart2/LegendPosition.hpp>
#include <com/sun/star/chart/ChartLegendExpansion.hpp>
#include <com/sun/star/chart2/RelativePosition.hpp>
#include <com/sun/star/chart2/RelativeSize.hpp>
#include "chartview/ExplicitValueProvider.hxx"

// header for class Rectangle
#include <tools/gen.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>

//.............................................................................
namespace chart
{
//.............................................................................
using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

bool PositionAndSizeHelper::moveObject( ObjectType eObjectType
                , const uno::Reference< beans::XPropertySet >& xObjectProp
                , const awt::Rectangle& rNewPositionAndSize
                , const awt::Rectangle& rPageRectangle
                )
{
    if(!xObjectProp.is())
        return false;
    Rectangle aObjectRect( Point(rNewPositionAndSize.X,rNewPositionAndSize.Y), Size(rNewPositionAndSize.Width,rNewPositionAndSize.Height) );
    Rectangle aPageRect( Point(rPageRectangle.X,rPageRectangle.Y), Size(rPageRectangle.Width,rPageRectangle.Height) );

    if( OBJECTTYPE_TITLE==eObjectType )
    {
        //@todo decide wether x is primary or secondary
        chart2::RelativePosition aRelativePosition;
        aRelativePosition.Anchor = drawing::Alignment_CENTER;
        //the anchor point at the title object is top/middle
        Point aPos = aObjectRect.TopLeft();
        aRelativePosition.Primary = (double(aPos.X())+double(aObjectRect.getWidth())/2.0)/double(aPageRect.getWidth());
        aRelativePosition.Secondary = (double(aPos.Y())+double(aObjectRect.getHeight())/2.0)/double(aPageRect.getHeight());
        xObjectProp->setPropertyValue( C2U( "RelativePosition" ), uno::makeAny(aRelativePosition) );
    }
    else if( OBJECTTYPE_DATA_CURVE_EQUATION==eObjectType )
    {
        //@todo decide wether x is primary or secondary
        chart2::RelativePosition aRelativePosition;
        aRelativePosition.Anchor = drawing::Alignment_TOP_LEFT;
        //the anchor point at the title object is top/middle
        Point aPos = aObjectRect.TopLeft();
        aRelativePosition.Primary = double(aPos.X())/double(aPageRect.getWidth());
        aRelativePosition.Secondary = double(aPos.Y())/double(aPageRect.getHeight());
        xObjectProp->setPropertyValue( C2U( "RelativePosition" ), uno::makeAny(aRelativePosition) );
    }
    else if(OBJECTTYPE_LEGEND==eObjectType)
    {
        xObjectProp->setPropertyValue( C2U( "AnchorPosition" ), uno::makeAny(LegendPosition(LegendPosition_CUSTOM)));
        xObjectProp->setPropertyValue( C2U( "Expansion" ), uno::makeAny(::com::sun::star::chart::ChartLegendExpansion_CUSTOM));
        chart2::RelativePosition aRelativePosition;
        chart2::RelativeSize aRelativeSize;
        Point aAnchor = aObjectRect.TopLeft();

        aRelativePosition.Primary =
            static_cast< double >( aAnchor.X()) /
            static_cast< double >( aPageRect.getWidth() );
        aRelativePosition.Secondary =
            static_cast< double >( aAnchor.Y()) /
            static_cast< double >( aPageRect.getHeight());

        xObjectProp->setPropertyValue( C2U( "RelativePosition" ), uno::makeAny(aRelativePosition) );
        
        aRelativeSize.Primary =
            static_cast< double >( aObjectRect.getWidth()) /
            static_cast< double >( aPageRect.getWidth() );
        if (aRelativeSize.Primary > 1.0)
            aRelativeSize.Primary = 1.0;
        aRelativeSize.Secondary =
            static_cast< double >( aObjectRect.getHeight()) /
            static_cast< double >( aPageRect.getHeight());
        if (aRelativeSize.Secondary > 1.0)
            aRelativeSize.Secondary = 1.0;

        xObjectProp->setPropertyValue( C2U( "RelativeSize" ), uno::makeAny(aRelativeSize) );        
    }
    else if(OBJECTTYPE_DIAGRAM==eObjectType || OBJECTTYPE_DIAGRAM_WALL==eObjectType || OBJECTTYPE_DIAGRAM_FLOOR==eObjectType)
    {
        //@todo decide wether x is primary or secondary

        //set position:
        chart2::RelativePosition aRelativePosition;
        aRelativePosition.Anchor = drawing::Alignment_CENTER;

        Point aPos = aObjectRect.Center();
        aRelativePosition.Primary = double(aPos.X())/double(aPageRect.getWidth());
        aRelativePosition.Secondary = double(aPos.Y())/double(aPageRect.getHeight());
        xObjectProp->setPropertyValue( C2U( "RelativePosition" ), uno::makeAny(aRelativePosition) );

        //set size:
        RelativeSize aRelativeSize;
        //the anchor points for the diagram are in the middle of the diagram
        //and in the middle of the page
        aRelativeSize.Primary = double(aObjectRect.getWidth())/double(aPageRect.getWidth());
        aRelativeSize.Secondary = double(aObjectRect.getHeight())/double(aPageRect.getHeight());
        xObjectProp->setPropertyValue( C2U( "RelativeSize" ), uno::makeAny(aRelativeSize) );
    }
    else
        return false;
    return true;
}

bool PositionAndSizeHelper::moveObject( const rtl::OUString& rObjectCID
                , const uno::Reference< frame::XModel >& xChartModel
                , const awt::Rectangle& rNewPositionAndSize
                , const awt::Rectangle& rPageRectangle
                )
{
    ControllerLockGuard aLockedControllers( xChartModel );

    awt::Rectangle aNewPositionAndSize( rNewPositionAndSize );

    uno::Reference< beans::XPropertySet > xObjectProp = ObjectIdentifier::getObjectPropertySet( rObjectCID, xChartModel );
    ObjectType eObjectType( ObjectIdentifier::getObjectType( rObjectCID ) );
    if(OBJECTTYPE_DIAGRAM==eObjectType || OBJECTTYPE_DIAGRAM_WALL==eObjectType || OBJECTTYPE_DIAGRAM_FLOOR==eObjectType)
    {
        xObjectProp = uno::Reference< beans::XPropertySet >( ObjectIdentifier::getDiagramForCID( rObjectCID, xChartModel ), uno::UNO_QUERY );
        if(!xObjectProp.is())
            return false;
    }
    return moveObject( eObjectType, xObjectProp, aNewPositionAndSize, rPageRectangle );
}

//.............................................................................
} //namespace chart
//.............................................................................
