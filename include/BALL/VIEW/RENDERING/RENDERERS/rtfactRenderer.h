// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_VIEW_RENDERING_RENDERERS_RTFACTRENDERER_H
#define BALL_VIEW_RENDERING_RENDERERS_RTFACTRENDERER_H

#define BALLVIEW_RTFACTRENDERER_THROW(exceptionName, message) (throw BALL::Exception::##exceptionName##(__FILE__, __LINE__, message))
#include <BALL/VIEW/RENDERING/RENDERERS/raytracingRenderer.h>

#include <BALL/VIEW/WIDGETS/scene.h>
#include <BALL/VIEW/KERNEL/stage.h>
#include <BALL/VIEW/KERNEL/representationManager.h>
#include <BALL/VIEW/PRIMITIVES/mesh.h>
#include <BALL/VIEW/KERNEL/mainControl.h>
#include <BALL/MATHS/surface.h>
#include <BALL/MATHS/vector3.h>
#include <BALL/DATATYPE/hashMap.h>

//RTRemote proxy to RTfact
#include <RTremote/Renderer.hpp>
#include <RTremote/Picking.hpp>

namespace BALL
{
  namespace VIEW
	{
		/** RTfactRenderer
			Provides ray tracing through RTfact-RTRemote
			\ingroup ViewRendering
			*/
		class BALL_VIEW_EXPORT RTfactRenderer
			: public RaytracingRenderer
		{	  
			public:	  

				/** This class encapsulates RTfact's data structures per Representation.
				 */
				class RTfactData
				{
					public:
						/// The top-level group handles this object was assigned to
						std::vector<RTfact::Remote::GroupHandle> top_group_handles;
						
						/// The object handles
						std::vector<RTfact::Remote::GeoHandle> object_handles;

						/// The materials
						std::vector<RTfact::Remote::RTAppearanceHandle> material_handles;

						/// Mark previously disabled representations
						bool has_been_disabled;
				};

				/// Default Constructor.
				RTfactRenderer()
					: RaytracingRenderer(),
						rtfact_needs_update_(false)
				{
				}

				/// Destructor
				virtual ~RTfactRenderer()
				{
				}

				/************************************************************************/
				/* RaytracingRenderer methods					   */
				/************************************************************************/
				virtual bool init(Scene& scene);

				virtual String getRenderer()  
				{
					return "RTfact-RTRemote Ray Tracer";
				}

				virtual void formatUpdated()
				{
				}

				virtual GeometricObject* pickObject(Position x, Position y);
				virtual void pickObjects(Position x1, Position y1, Position x2, Position y2,
				                         std::list<GeometricObject*>& objects);

				virtual void setSize(float width, float height);
				
				virtual void setupStereo(float eye_separation, float focal_length);

				virtual void getFrustum(float& near_f, float& far_f, float& left_f, float& right_f, float& top_f, float& bottom_f);
				virtual void setFrustum(float near_f, float far_f, float left_f, float right_f, float top_f, float bottom_f);

				virtual void prepareBufferedRendering(const Stage& stage);
				virtual void renderToBufferImpl(FrameBufferPtr buffer);

				virtual void useContinuousLoop(bool use_loop);

				void bufferRepresentation(const Representation& rep);
				void removeRepresentation(const Representation& rep);

				void setLights(bool reset_all = false);

				void updateCamera(const Camera* camera = 0);
				
				void updateBackgroundColor();

				void setupEnvironmentMap(const QImage& image);

				void updateMaterialForRepresentation(Representation const* rep);

				RTfact::Remote::GroupHandle transformTube(const TwoColoredTube& tube);
				RTfact::Remote::GroupHandle transformLine(const TwoColoredLine& line);
				void updateMaterialFromStage(RTfact::Remote::RTAppearanceHandle& material);
				void convertMaterial(Stage::RaytracingMaterial const& rt_material, RTfact::Remote::RTAppearanceHandle& material);

				/** Raytracing-related functionality **/
				//@{ 
				/** Intersect a set of rays with the geometry buffered by this renderer.
				 *
				 *  This function will intersect the rays 
				 *
				 *     origins[i] + l * directions[i]
				 *
				 *  with the geometry that has been buffered by this renderer previously.
				 */
				virtual std::vector<float> intersectRaysWithGeometry(const std::vector<Vector3>& origins, 
																														 const std::vector<Vector3>& directions);

				//@}

			private:

				std::vector<RTfact::Remote::RTLightHandle> lights_;

				RTfact::Remote::Renderer m_renderer;

				boost::shared_ptr<RTfact::Remote::Picking>  m_picking;

				HashMap<Representation const*, RTfactData> objects_;
				HashMap<RTfact::Remote::GeoHandle, GeometricObject*> geometric_objects_;

				Surface sphere_template_;
				Surface tube_template_;

				template<typename taPixelType>
				void renderImpl(taPixelType* buffer, const unsigned int width, const unsigned int height, const unsigned int pitch);

				void renderImpl(float* buffer, const unsigned int width, const unsigned int height, const unsigned int pitch)
				{
				}

				bool rtfact_needs_update_;

				Vector3 last_camera_position;
				Vector3 last_camera_view_vec;
				Vector3 last_camera_lookup;

				float x_scale_;
				float y_scale_;
		};


	} // namespace VIEW
} // namespace BALL

#endif // BALL_VIEW_RENDERING_RTFACTRENDERER_H