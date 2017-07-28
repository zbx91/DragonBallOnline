//**********************************************************************
//
// Copyright (c) 2002-2005
// PathEngine
// Lyon, France
//
// All Rights Reserved
//
//**********************************************************************

#ifndef MESH_RENDER_INFO_INCLUDED
#define MESH_RENDER_INFO_INCLUDED

#include "libs/Geometry/interface/Vector3F.h"
#include "common/TemplateMesh/Helper.h"
#include "common/STL/vector.h"
#include "common/interface/Assert.h"

class iGraphicsInterface;
class iRenderGeometry;
class cGeometrySet;

class cMeshRenderInfo
{
    cGeometrySet* _geometrySet;
    std::vector<iRenderGeometry*> _edgeElements;
    std::vector<iRenderGeometry*> _faceElements;
    iGraphicsInterface* _gip;

public:
    
    cMeshRenderInfo()
    {
        _geometrySet = 0;
        _gip = 0;
    }
    ~cMeshRenderInfo();

    template <class tMesh> void
    build(tMesh& mesh, iGraphicsInterface* gip)
    {
        typedef typename tMesh::face tFace;
        typedef typename tMesh::edge tEdge;

        _gip=gip;
        clear();

        _geometrySet = gip->newGeometrySet();

        tFace f=mesh.beginFace();

        _faceElements.resize(mesh.faces());

        for(f=mesh.beginFace();f.valid();++f)
        {
            assertD(f.index()>=0 && f.index()<mesh.faces());
            if(f->isExternal())
            {
                _faceElements[f.index()]=0;
                continue;
            }
            tEdge e;
            e = mesh.beginEdge(f);
            long edgesAround = CountEdgesAround(e);
            std::vector<float> pointarray(edgesAround * 3);
            float* ptr;
            ptr = &pointarray.front();
            do
            {
                cVector3F p = GetVertexPoint3F(e);
                *ptr++ = p.x();
                *ptr++ = p.y();
                *ptr++ = p.z();
            }
            while(++e != mesh.beginEdge(f));
            _faceElements[f.index()]=gip->newPolygon(edgesAround, &pointarray.front(), _geometrySet);
        }
        for(f=mesh.beginFace();f.valid();++f)
        {
            tEdge e=mesh.beginEdge(f);
            do
            {
                float data[6];
                float* ptr = data;
                tEdge nexte = e.next();
                cVector3F p;
                p = GetVertexPoint3F(e);
                *ptr++ = p.x();
                *ptr++ = p.y();
                *ptr++ = p.z();
                p = GetVertexPoint3F(nexte);
                *ptr++ = p.x();
                *ptr++ = p.y();
                *ptr++ = p.z();
                if(e.index() >= SizeL(_edgeElements))
                {
                    _edgeElements.resize(e.index()+1);
                }
                _edgeElements[e.index()] = gip->newLine(data, _geometrySet);
            }
            while(++e!=mesh.beginEdge(f));
        }
    }

    template <class tMesh>
    cMeshRenderInfo(tMesh& mesh, iGraphicsInterface* gip)
    {
        _geometrySet = 0;
        _gip = 0;
        build(mesh,gip);
    }

    void clear();

    void drawFaces() const;
    void drawEdges() const;
    void drawFace(long index) const;
    void drawEdge(long index) const;
};

#endif