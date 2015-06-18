#pragma once

class CRawMesh;

void ContructBoxVertexBuffer(CRawModel *pRawModel);
void ContructSphereVertexBuffer(CRawModel *pRawModel);
void ContructRegularGrid(XMINT2 const &_vGeometryDetail, CRawModel *pRawModel);