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



#ifndef INCLUDED_PDFI_DRAWTREEVISITING_HXX
#define INCLUDED_PDFI_DRAWTREEVISITING_HXX

#include "treevisiting.hxx"

#include "com/sun/star/i18n/XBreakIterator.hpp"
#include "com/sun/star/i18n/XCharacterClassification.hpp"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"

namespace pdfi
{
    struct DrawElement;

    class DrawXmlOptimizer : public ElementTreeVisitor
    {
    private:
        PDFIProcessor& m_rProcessor;
        void optimizeTextElements(Element& rParent);

    public:
	::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator > mxBreakIter;
	const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator >& GetBreakIterator();
        explicit DrawXmlOptimizer(PDFIProcessor& rProcessor) :
            m_rProcessor(rProcessor)
        {}

        virtual void visit( HyperlinkElement&, const std::list< Element* >::const_iterator& );
        virtual void visit( TextElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ParagraphElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( FrameElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PolyPolyElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ImageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( DocumentElement&, const std::list< Element* >::const_iterator&  );
    };
   
    class DrawXmlFinalizer : public ElementTreeVisitor
    {
    private:
        StyleContainer& m_rStyleContainer;
        PDFIProcessor&  m_rProcessor;

    public:
        explicit DrawXmlFinalizer(StyleContainer& rStyleContainer,
                                  PDFIProcessor&  rProcessor) :
            m_rStyleContainer(rStyleContainer),
            m_rProcessor(rProcessor)
        {}

        virtual void visit( HyperlinkElement&, const std::list< Element* >::const_iterator& );
        virtual void visit( TextElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ParagraphElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( FrameElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PolyPolyElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ImageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( DocumentElement&, const std::list< Element* >::const_iterator&  );
    };
   
    class DrawXmlEmitter : public ElementTreeVisitor
    {
    private:
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xFactory;
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > xCtx;
	::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator > mxBreakIter;
	::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCharacterClassification > mxCharClass;

	    PDFIProcessor&  m_rProcessor;

        EmitContext& m_rEmitContext ;
        /// writes Impress doc when false
        const bool   m_bWriteDrawDocument;

        void fillFrameProps( DrawElement&       rElem, 
                             PropertyMap&       rProps, 
                             const EmitContext& rEmitContext,
                             bool               bWasTransformed = false
                             );

    public:
	const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator >& GetBreakIterator();
	const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCharacterClassification >& GetCharacterClassification();
        enum DocType{ DRAW_DOC, IMPRESS_DOC };
        explicit DrawXmlEmitter(EmitContext& rEmitContext, DocType eDocType, PDFIProcessor& rProc ) :
            m_rProcessor( rProc ),
            m_rEmitContext(rEmitContext),
            m_bWriteDrawDocument(eDocType==DRAW_DOC)
        {}

        virtual void visit( HyperlinkElement&, const std::list< Element* >::const_iterator& );
        virtual void visit( TextElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ParagraphElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( FrameElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PolyPolyElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( ImageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( PageElement&, const std::list< Element* >::const_iterator&  );
        virtual void visit( DocumentElement&, const std::list< Element* >::const_iterator&  );
    };   
}

#endif
