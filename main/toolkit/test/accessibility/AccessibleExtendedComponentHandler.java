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

import com.sun.star.uno.UnoRuntime;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.accessibility.XAccessibleExtendedComponent;


class AccessibleExtendedComponentHandler 
    extends NodeHandler
{
    public NodeHandler createHandler (XAccessibleContext xContext)
    {
        XAccessibleExtendedComponent xEComponent = 
            (XAccessibleExtendedComponent) UnoRuntime.queryInterface (
                XAccessibleExtendedComponent.class, xContext);
        if (xEComponent != null)
            return new AccessibleExtendedComponentHandler (xEComponent);
        else
            return null;
    }

    public AccessibleExtendedComponentHandler ()
    {
    }

    public AccessibleExtendedComponentHandler (XAccessibleExtendedComponent xEComponent)
    {
        if (xEComponent != null)
            maChildList.setSize (0);
    }

    private static XAccessibleExtendedComponent getComponent (AccTreeNode aNode)
    {
        return (XAccessibleExtendedComponent) UnoRuntime.queryInterface (
            XAccessibleExtendedComponent.class, 
            aNode.getContext());
    }


    public AccessibleTreeNode createChild (AccessibleTreeNode aParent, int nIndex)
    {
        AccessibleTreeNode aChild = null;
        if (aParent instanceof AccTreeNode)
        {
            XAccessibleExtendedComponent xEComponent = getComponent ((AccTreeNode)aParent);
        
            if (xEComponent != null)
            {
                int nColor;
                switch( nIndex )
                {
                    case 0:
                        nColor = xEComponent.getForeground();
                        aChild = new StringNode ("Depricated Foreground color: R"
                            +       (nColor>>16&0xff)
                            + "G" + (nColor>>8&0xff)
                            + "B" + (nColor>>0&0xff)
                            + "A" + (nColor>>24&0xff),
                            aParent);
                        break;
                    case 1:
                        nColor = xEComponent.getBackground();
                        aChild = new StringNode ("Depricated Background color: R"
                            +       (nColor>>16&0xff)
                            + "G" + (nColor>>8&0xff)
                            + "B" + (nColor>>0&0xff)
                            + "A" + (nColor>>24&0xff),
                            aParent);
                        break;
                }
            }
        }
        return aChild;
    }
}
