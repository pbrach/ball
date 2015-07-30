#ifndef SDFInputDialog_H
#define SDFInputDialog_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include <SDFInputDataItem.h>

namespace BALL
{
	namespace VIEW
	{
		/** @class SDFInputDialog
		* @brief a dialog
		*
		* @todo
		*/
		class SDFInputDialog : public QDialog
		{
		
			Q_OBJECT
		public:
			/** @name Constructors and Destructors
			*/
			SDFInputDialog(SDFInputDataItem* item);
			SDFInputDialog();
			~SDFInputDialog();
		
			/** @name Accessors
			*/
		
			/** returns the activity numbers
			*/ 
			std::multiset<int> numbers();
		
			bool centerDescriptorValues();
			bool centerResponseValues();
			bool inputOk();
		
		public slots:
			void getNumbers();
			void classNamesChange();

		private:
		
			/** @name Attributes
			*/
			QLineEdit* activity_edit_;
			QCheckBox* sd_descriptors_checkbox_;
			QCheckBox* center_descriptor_values_;
			QCheckBox* center_response_values_;
			QCheckBox* class_names_checkbox_;
			std::multiset<int> numbers_;
			bool input_ok_;
			SDFInputDataItem* input_item_;
			vector<String>* property_names_;

		};
	}
}

 #endif
