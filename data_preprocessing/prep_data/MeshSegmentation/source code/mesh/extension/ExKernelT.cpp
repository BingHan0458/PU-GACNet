
#include "ExKernelT.h"
#include <stdio.h>
#include <iostream>
#include "algorithm"
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <set>
#include <list>
#include <time.h>

using namespace Eigen;
using namespace std;


namespace MeshN { 

	///////////////////////////////////////////////////////////////////////////////
	// Implementation of member functions of ExKernelT
	/////////////////////////////////////////////////////////////////////////////// 
	template<class ExItems>
	ExKernelT<ExItems>::ExKernelT(): KernelT<ExItems>() {

		//kdtree_ = NULL;
		//ps_     = NULL;
		isNormal_ = false;
		isArea_ = false;

	}
	//////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	ExKernelT<ExItems>::~ExKernelT(){

		//if(kdtree_ != NULL) delete kdtree_;

		//if(ps_ != NULL)     delete ps_;

	}
	//////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void ExKernelT<ExItems>::meshInit(){

		//update_normals();
		update_facet_normals();
		//update_vertex_normals_max();
		//update_edge_length();
		//createPS();
		//createKdTree();

	}

	//////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void ExKernelT<ExItems>::MeshSegment(string modelName, int patchNum, float patchRadius)
	{
		int pointNum = vertex_size();
		list<int> l;
		list<int>::iterator it;
		srand((unsigned)time(NULL));
		while (l.size()!=patchNum)
		{
			l.push_back(rand() % pointNum);
			l.sort();
			l.unique();
		}

		int patchIdx = 1;
		for (it = l.begin(); it != l.end(); ++it)
		{
			vector<VertexHandle> patchVhs;
			vector<int> faceId1; vector<int> faceId2; vector<int> faceId3;
			VertexIterator vi = vertex_begin();
			for (; vi != vertex_end(); vi++)
			{
				VertexHandle vh = vertex_handle(vi->halfedge_handle_);
				if (vh.idx() == *it)
				{
					getPatch(vh, patchRadius, patchVhs);
				}
			}
			FacetIterator fi(facet_begin());
			for (; fi != facet_end(); fi++)
			{
				HalfedgeHandle hh = fi->halfedge_handle_;
				HalfedgeHandle nh = next_halfedge_handle(hh);
				HalfedgeHandle nnh = next_halfedge_handle(nh);

				VertexHandle vh = vertex_handle(hh);
				VertexHandle nvh = vertex_handle(nh);
				VertexHandle nnvh = vertex_handle(nnh);

				int flag = 0;
				for (int i = 0; i < patchVhs.size(); i++)
				{
					if (vh == patchVhs[i]) { flag += 1; }
					if (nvh == patchVhs[i]) { flag += 1; }
					if (nnvh == patchVhs[i]) { flag += 1; }
				}
				if (flag == 3)
				{
					for (int i = 0; i < patchVhs.size(); i++)
					{
						if (vh == patchVhs[i]) { faceId1.push_back(i); }
						if (nvh == patchVhs[i]) { faceId2.push_back(i); }
						if (nnvh == patchVhs[i]) { faceId3.push_back(i); }
					}
				}
			}
			
			// write to file
			int patchVertexNum = patchVhs.size();
			int patchFaceNum = faceId1.size();
			FILE *fp;
			char fileName[50];
			sprintf(fileName, "%s_%d.off", modelName.c_str(), patchIdx);
			fp = fopen(fileName, "w");
			fprintf(fp, "OFF\n");
			fprintf(fp, "%d %d %d\n", patchVertexNum, patchFaceNum, 0);
			for (int i = 0; i < patchVhs.size(); i++)
			{
				VertexHandle vh = patchVhs[i];
				fprintf(fp, "%f %f %f\n", coord(vh).data_[0], coord(vh).data_[1], coord(vh).data_[2]);
			}
			for (int i = 0; i < faceId1.size(); i++)
			{
				fprintf(fp, "3 %d %d %d\n", faceId1[i], faceId2[i], faceId3[i]);
			}
			fclose(fp);
			patchIdx += 1;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	typename ExKernelT<ExItems>::Scalar
		ExKernelT<ExItems>::calc_facet_area(const FacetHandle& _fh){

			HalfedgeHandle& hh0 = halfedge_handle(_fh);
			HalfedgeHandle& hh1 = next_halfedge_handle(hh0);
			HalfedgeHandle& hh2 = next_halfedge_handle(hh1);

			EdgeHandle& eh0 = edge_handle(hh0);
			EdgeHandle& eh1 = edge_handle(hh1);
			EdgeHandle& eh2 = edge_handle(hh2);

			Scalar eh0_length = calc_edge_length(eh0);
			Scalar eh1_length = calc_edge_length(eh1);
			Scalar eh2_length = calc_edge_length(eh2);

			Scalar area;
			Scalar p = (eh0_length + eh1_length + eh2_length)/2.0;
			area=sqrt( p * (p-eh0_length) * (p-eh1_length) * (p-eh2_length) );

			facet_ref(_fh).area_ = area;
			return area;
	}

	//////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void ExKernelT<ExItems>::update_area()
	{   
		set_isArea(true);
		FacetIterator fit(facet_begin() );
		for(;fit<facet_end(); fit++)
		{
			HalfedgeHandle hh = fit->halfedge_handle_;
			FacetHandle    fh = facet_handle(hh);
			calc_facet_area(fh);
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	typename ExKernelT<ExItems>::Scalar
		ExKernelT<ExItems>::get_area(const FacetHandle& _fh)
	{
		return facet_ref(_fh).area_;
	}
	//////////////////////////////////////////////////////////////////////////////
	template<class ExItems> 
	double 
		ExKernelT<ExItems>::calc_edge_length(const EdgeHandle &_eh) {//updating all edge length


			HalfedgeHandle& h1 = halfedge_handle(_eh,0);
			HalfedgeHandle& h2 = halfedge_handle(_eh,1);

			Vertex v0 = vertex_ref(vertex_handle(h1) );
			Vertex v1 = vertex_ref(vertex_handle(h2) );

			return (v0.coord_-v1.coord_).norm();

	}

	template<class ExItems>
	void
		ExKernelT<ExItems>::getNeighborRing(VertexHandle& _vh, int _ring, std::vector<VertexHandle>& NeighborRing){//得到第ring环邻接点

			NeighborRing.push_back( _vh );
			int iteration = 0;
			int verNewNum = NeighborRing.size();
			int verOldNum = verNewNum-1;
			int verOldNum1 = verOldNum;

			do{
				verOldNum = NeighborRing.size();
				for(int i=verOldNum1; i<verNewNum; i++){
					VertexHandle& vh = NeighborRing[i];					
					HalfedgeHandle& hh = halfedge_handle(vh);
					HalfedgeHandle css(hh);
					do{
						int ii = 0;
						HalfedgeHandle& opp_hh = opposite_halfedge_handle(css);
						VertexHandle&   opp_vh = vertex_handle(opp_hh);
						for(ii=0; ii<NeighborRing.size(); ii++)
							if(opp_vh == NeighborRing[ii] ) break;

						if(ii >= NeighborRing.size() )
							NeighborRing.push_back(opp_vh);

						css = cw_rotated(css);
					}while(css != hh);
				}

				verNewNum = NeighborRing.size();
				verOldNum1 = verOldNum;
				iteration++;
			}while(iteration < _ring);

	}

	template<class ExItems>
	void
		ExKernelT<ExItems>::getPatch(VertexHandle& _vh, float patchRadius, std::vector<VertexHandle>& NeighborRing) {

		NeighborRing.push_back(_vh);
		int iteration = 0;
		int verNewNum = NeighborRing.size();
		int verOldNum = verNewNum - 1;
		int verOldNum1 = verOldNum;
		vector<float> ringRadius;
		float currentPatchRadius = 0;

		do {
			verOldNum = NeighborRing.size();
			for (int i = verOldNum1; i<verNewNum; i++) {
				VertexHandle& vh = NeighborRing[i];
				HalfedgeHandle& hh = halfedge_handle(vh);
				HalfedgeHandle css(hh);
				do {
					int ii = 0;
					HalfedgeHandle& opp_hh = opposite_halfedge_handle(css);
					VertexHandle&   opp_vh = vertex_handle(opp_hh);
					for (ii = 0; ii<NeighborRing.size(); ii++)
						if (opp_vh == NeighborRing[ii]) break;

					if (ii >= NeighborRing.size())
					{
						NeighborRing.push_back(opp_vh);
					}						

					css = cw_rotated(css);
				} while (css != hh);

				if (iteration == 0 || iteration == ringRadius.size())
				{
					Coord v0 = coord(NeighborRing[i]);
					float edge_length_sum = 0;
					for (int j = i + 1; j < NeighborRing.size(); j++)
					{
						Coord v1 = coord(NeighborRing[j]);
						edge_length_sum += (v0 - v1).norm();
					}
					float edge_length_averg = edge_length_sum / (NeighborRing.size() - i);
					ringRadius.push_back(edge_length_averg);
				}
			}

			verNewNum = NeighborRing.size();
			verOldNum1 = verOldNum;
			iteration++;
			for (int jj = 0; jj < ringRadius.size(); jj++)
			{
				currentPatchRadius += ringRadius[jj];
			}

		} while (currentPatchRadius < patchRadius);

	}


	template<class ExItems>
	void
		ExKernelT<ExItems>::getNeighborVertices(VertexHandle& _vh, int _verticesNum, std::vector<VertexHandle>& NeighborVertices) {

		NeighborVertices.push_back(_vh);
		int iteration = 0;
		int verNewNum = NeighborVertices.size();
		int verOldNum = verNewNum - 1;
		int verOldNum1 = verOldNum;

		while (NeighborVertices.size() <= _verticesNum)
		{
			verOldNum = NeighborVertices.size();
			for (int i = verOldNum1; i<verNewNum; i++) {
				VertexHandle& vh = NeighborVertices[i];
				HalfedgeHandle& hh = halfedge_handle(vh);
				HalfedgeHandle css(hh);
				do {
					int ii = 0;
					HalfedgeHandle& opp_hh = opposite_halfedge_handle(css);
					VertexHandle&   opp_vh = vertex_handle(opp_hh);
					for (ii = 0; ii<NeighborVertices.size(); ii++)
						if (opp_vh == NeighborVertices[ii]) break;

					if (ii >= NeighborVertices.size())
						NeighborVertices.push_back(opp_vh);
					if (NeighborVertices.size() > _verticesNum) break;

					css = cw_rotated(css);
				} while (css != hh);
				if (NeighborVertices.size() > _verticesNum) break;
			}
			if (NeighborVertices.size() > _verticesNum) break;
			verNewNum = NeighborVertices.size();
			verOldNum1 = verOldNum;
		}

	}
	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void
		ExKernelT<ExItems>::getNeighborFaceN1(FacetHandle& _fh, std::vector<FacetHandle>& _fhs){//sharing common edges

			HalfedgeHandle& hh = halfedge_handle(_fh);
			HalfedgeHandle& pre_hh = prev_halfedge_handle(hh);
			HalfedgeHandle& nex_hh = next_halfedge_handle(hh);

			_fhs.push_back( facet_handle( opposite_halfedge_handle(hh) ) );
			_fhs.push_back( facet_handle( opposite_halfedge_handle(pre_hh ) ) );
			_fhs.push_back( facet_handle(opposite_halfedge_handle(nex_hh ) ) );
	}
	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void 
		ExKernelT<ExItems>::getNeighborFaceN2(FacetHandle& _fh, std::vector<FacetHandle>& _fhs){//sharing common vertices

			HalfedgeHandle& hh = halfedge_handle(_fh);
			HalfedgeHandle& pre_hh = prev_halfedge_handle(hh);
			HalfedgeHandle& nex_hh = next_halfedge_handle(hh);

			VertexHandle  vhs[3];
			vhs[0] = vertex_handle(hh);
			vhs[1] = vertex_handle(pre_hh);
			vhs[2] = vertex_handle(nex_hh);

			for(int i=0; i<3; i++){

				HalfedgeHandle& hhv = halfedge_handle( vhs[i] );
				HalfedgeHandle cursor(hhv);

				do{

					FacetHandle fh = facet_handle(cursor);
					if(fh.is_valid() && fh != _fh){

						if(_fhs.size() != 0){
							int j;
							for(j=0; j< _fhs.size(); j++){

								if(fh.idx() == _fhs[j].idx() ) break;
							}//end for

							if(j>= _fhs.size() ) _fhs.push_back(fh);

						}//end if
						else _fhs.push_back(fh);
					}//end if

					cursor = cw_rotated(cursor);

				}while(hhv != cursor);//end for do while
			}//end for

	}
	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void ExKernelT<ExItems>::output_to_file(){

		FILE *fp;
		fp=fopen("result.off","w");

		int no_vertex=vertex_size();
		int no_facet=facet_size();
		int edge = 0;

		fprintf(fp,"OFF\n");
		fprintf(fp,"%d  %d %d\n",no_vertex,no_facet, edge);

		VertexIterator vit(vertex_begin());

		for(;vit!=vertex_end();vit++){

			VertexHandle vh=vertex_handle(vit->halfedge_handle_);
			fprintf(fp," %f  %f  %f\n",coord(vh).data_[0],coord(vh).data_[1],coord(vh).data_[2]);

		}

		FacetIterator fit(facet_begin());

		for(;fit!=facet_end();fit++){

			HalfedgeHandle hh=fit->halfedge_handle_;
			HalfedgeHandle nh=next_halfedge_handle(hh);
			HalfedgeHandle nnh=next_halfedge_handle(nh);

			VertexHandle vh=vertex_handle(hh);
			VertexHandle nvh=vertex_handle(nh);
			VertexHandle nnvh=vertex_handle(nnh);

			fprintf(fp,"3 %d  %d  %d\n",vh.idx(),nvh.idx(),nnvh.idx());

		}

		fclose(fp);
	}
	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	void ExKernelT<ExItems>::output_to_file(char* filename){

		FILE *fp;
		fp = fopen(filename, "w");

		int no_vertex = vertex_size();
		int no_facet = facet_size();
		int edge = 0;

		fprintf(fp, "OFF\n");
		fprintf(fp, "%d  %d %d\n", no_vertex, no_facet, edge);

		VertexIterator vit(vertex_begin());

		for (; vit != vertex_end(); vit++){

			VertexHandle vh = vertex_handle(vit->halfedge_handle_);
			fprintf(fp, " %f  %f  %f\n", coord(vh).data_[0], coord(vh).data_[1], coord(vh).data_[2]);

		}

		FacetIterator fit(facet_begin());

		for (; fit != facet_end(); fit++){

			HalfedgeHandle hh = fit->halfedge_handle_;
			HalfedgeHandle nh = next_halfedge_handle(hh);
			HalfedgeHandle nnh = next_halfedge_handle(nh);

			VertexHandle vh = vertex_handle(hh);
			VertexHandle nvh = vertex_handle(nh);
			VertexHandle nnvh = vertex_handle(nnh);

			fprintf(fp, "3 %d  %d  %d\n", vh.idx(), nvh.idx(), nnvh.idx());

		}

		fclose(fp);
	}
//////////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	typename ExKernelT<ExItems>::Normal
		ExKernelT<ExItems>::normal(const FacetHandle& _fh) {
			assert( _fh.is_valid() );
			assert( _fh.idx() < facet_size() );
			return facet_ref(_fh).normal_;
	}


	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	typename ExKernelT<ExItems>::Normal
		ExKernelT<ExItems>::calc_normal(const FacetHandle& _fh) {
			assert( _fh.is_valid() );
			assert( _fh.idx() < facet_size() );

			const HalfedgeHandle&   hh = halfedge_handle(_fh);
			const HalfedgeHandle& p_hh = prev_halfedge_handle(hh);
			const HalfedgeHandle& n_hh = next_halfedge_handle(hh);

			const Coord& cd0 = coord( vertex_handle( hh) );
			const Coord& cd1 = coord( vertex_handle(p_hh) );
			const Coord& cd2 = coord( vertex_handle(n_hh) );

			//return ((cd1-cd0)%(cd2-cd1)).normalize();
			return ((cd2-cd1)%(cd1-cd0)).normalize();//be careful
	}


	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	void ExKernelT<ExItems>::update_facet_normals(void) {

		set_isNormal(true);
		FacetIterator fi = facet_begin();

		for ( ; fi!=facet_end(); ++fi) {
			if (fi->status_.is_deleted()) continue;

			assert(fi->halfedge_handle_.is_valid());
			fi->normal_ = calc_normal( facet_handle(fi->halfedge_handle_) );
		}
	}


	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	typename ExKernelT<ExItems>::Normal
		ExKernelT<ExItems>::normal(const VertexHandle& _vh) {
			assert( _vh.is_valid() );
			assert( _vh.idx() < vertex_size() );
			return vertex_ref(_vh).normal_;
	}


	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	typename ExKernelT<ExItems>::Normal
		ExKernelT<ExItems>::calc_normal(const VertexHandle& _vh) {
			assert( _vh.is_valid());
			assert( _vh.idx() < vertex_size() );

			Normal          norm;
			HalfedgeHandle& hh = halfedge_handle(_vh);
			HalfedgeHandle  cs (hh);
			do {
				FacetHandle& fh = facet_handle(cs);
				if (fh.is_valid()) 
					norm  += normal(fh);

				cs = cw_rotated(cs);
			} while ( cs != hh );

			return norm.normalize();
	}

	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	void ExKernelT<ExItems>::update_vertex_normals(void) {
		VertexIterator vi = vertex_begin();

		for ( ; vi!=vertex_end(); ++vi) {
			if (vi->status_.is_deleted()) continue;

			assert(vi->halfedge_handle_.is_valid());
			vi->normal_ = calc_normal( vertex_handle(vi->halfedge_handle_) );
			//std::cout<<vi->normal_<<std::endl;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	typename ExKernelT<ExItems>::Normal
		ExKernelT<ExItems>::calc_normal_max(const VertexHandle& _vh){

			assert(_vh.is_valid() );
			assert(_vh.idx() < vertex_size() );

			HalfedgeHandle& hh = halfedge_handle(_vh);
			Coord& vc = vertex_ref(_vh).coord_;
			Normal n(0,0,0);
			HalfedgeHandle css(hh);
			do{
				FacetHandle& fh = facet_handle(css);
				if(fh.is_valid() ){

					HalfedgeHandle& nexhh = next_halfedge_handle(css);
					HalfedgeHandle& prehh = prev_halfedge_handle(css);

					VertexHandle& nvh = vertex_handle(nexhh);
					VertexHandle& prevh = vertex_handle(prehh);

					Coord& nvc = vertex_ref(nvh).coord_;
					Coord& prevc = vertex_ref(prevh).coord_;

					Coord vec1 = vc - nvc;
					Coord vec2 = vc - prevc;
					Coord vec12cross = vec1 % vec2;//cross multiplication

					float weight = vec12cross.length() / (vec1.sqLength() * vec2.sqLength() );

					n += facet_ref(fh).normal_ * weight;//---------------------------------------注意
					//n += calc_normal(fh);//---------------------------------注意

				}//if

				css = cw_rotated(css);
			}while(css != hh);

			n.normalize();

			return n;
	}
	//////////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	void ExKernelT<ExItems>::update_vertex_normals_max(void){

		VertexIterator vi = vertex_begin();

		for ( ; vi!=vertex_end(); ++vi) {
			if (vi->status_.is_deleted() ) continue;

			assert(vi->halfedge_handle_.is_valid());
			//vi->normal_ = calc_normal( vertex_handle(vi->halfedge_handle_));
			vi->normal_ = calc_normal_max( vertex_handle(vi->halfedge_handle_) );
			//std::cout<<vi->normal_<<std::endl;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	void ExKernelT<ExItems>::update_normals(void) {
		update_facet_normals();

		VertexIterator vi = vertex_begin();
		for ( ; vi!=vertex_end(); ++vi) {
			if (vi->status_.is_deleted()) continue;

			assert(vi->halfedge_handle_.is_valid());

			HalfedgeHandle& hh = vi->halfedge_handle_;
			VertexHandle&   vh = vertex_handle(hh);
			HalfedgeHandle  cs (hh);
			Normal          norm;
			do {
				const FacetHandle& fh = facet_handle(cs);
				if (fh.is_valid())  
					norm +=  facet_ref(fh).normal_;
				cs = cw_rotated( cs );
			} while ( cs != hh ); 

			vi->normal_ = norm.normalize();
		}


	}

	///////////////////////////////////////////////////////////////////////////////
	template <class ExItems> 
	void ExKernelT<ExItems>::update_edge_length(void) {//计算网格中边长信息
		float global_max_edge_length_ = 0;
		float averagedlength = 0.0;

		EdgeIterator eit = edge_begin();
		for ( ; eit!=edge_end(); ++eit ) {
			Vertex& v0 = vertex_ref( eit->halfedges_[0].vertex_handle_ );
			Vertex& v1 = vertex_ref( eit->halfedges_[1].vertex_handle_ );

			eit->length_ = (v0.coord_ - v1.coord_).norm();
			//std::cout<<eit->length_<<"\n";
			averagedlength += eit->length_;

			if (global_max_edge_length_ < eit->length_)
				global_max_edge_length_ = eit->length_; 
		} 

		averagedlength /= edge_size();
		set_average_edge_length(averagedlength);
	}
	/////////////////////////////////////////////////////////////////////////
	template<class ExItems>
	typename ExKernelT<ExItems>::Coord
		ExKernelT<ExItems>::calc_centroid(const FacetHandle& _fh){

			HalfedgeHandle& hh = halfedge_handle(_fh);
			HalfedgeHandle& n_hh = next_halfedge_handle(hh);
			HalfedgeHandle& pre_hh = prev_halfedge_handle(hh);

			VertexHandle& vh = vertex_handle(hh);
			VertexHandle& n_vh = vertex_handle(n_hh);
			VertexHandle& pre_vh = vertex_handle(pre_hh);

			return Coord(coord(vh) +
				coord(n_vh)+
				coord(pre_vh) )/3.0;

	}

} /// namespace




