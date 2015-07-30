#ifndef DATAITEMVIEW_H
#define DATAITEMVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>

#include <dataItemScene.h>




namespace BALL
{
	namespace VIEW
	{

		class MainWindow;
		class DataItem;
		class Edge;
		
		/** @class DataItemView
		* @brief class for viewing all DataItems
		*
		* @todo
		*/
		class DataItemView : public QGraphicsView
		{
		Q_OBJECT
		
		public:
			/** @name Constructors and Destructors */

			/** constructor
			* @param scene the scene connected to this view which contains the items
			* @param mw the main windwo to which this View belongs
			*/
			DataItemView(DataItemScene* scene, MainWindow* mw);


			/** @name Attributes */

			/** the scene*/
			DataItemScene* data_scene;

			/** name of the widget for which this object is used,i.e. "view", "model_list" or "source_list" */
			std::string name; 

			void addDropSite();
			
			void scaleView(qreal scaleFactor);
		

		
		protected:

			/** @name Event Handlers 
			* reimplemented event handlers of this class
			*/

			/** event handler for mouse scroll events 
			* needed for scaling the view
			*/
			void wheelEvent(QWheelEvent* event);

			/** event handler for keyboard key events 
			* used for moving the items on the view
			*/
			void keyPressEvent(QKeyEvent* event);
			
		
		private:
			int timer_id_;
		
		};
	}
}	

 #endif

