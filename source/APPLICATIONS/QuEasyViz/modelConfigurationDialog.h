
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <BALL/QSAR/registry.h>
#include <BALL/QSAR/Model.h>

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QDialogButtonBox>

#include <modelConfigurationDialogPages.h>
#include <mainWindow.h>
#include <modelItem.h>
#include <inputDataItem.h>

namespace BALL
{
	namespace VIEW
	{
		class ModelParameterPage;
		class KernelParameterPage;
		class OptimizePage;
		class ModelPropertiesPage;
		class ConnectionsPage;
		class DataPage;
		class MainWindow;
	
		/** @class ModelConfigurationDialog
		* @brief a dialog
		*
		* @todo
		*/
		class ModelConfigurationDialog : public QDialog
		{
			Q_OBJECT
		
			public:
				/** @name Constructors and Destructors*/
				/**standard constructor */
				ModelConfigurationDialog();

				/** constructor 
				@param modelitem 
				@param input
				*/
				ModelConfigurationDialog(ModelItem* modelitem, InputDataItem* input, MainWindow* parent);

				/** constructor 
				@param modelitem 
				*/
				ModelConfigurationDialog(ModelItem* modelitem, MainWindow* parent);

				/** destructor */
				~ModelConfigurationDialog();
		

				/** @name Accessors */

				/** returns the modelitem */
				ModelItem* modelItem();

				/** returns the registry entry */
				BALL::QSAR::RegistryEntry* entry();

				/** returns the first string of an individual kernel*/
				std::string KernelString1();

				/** returns the second string of an individual kernel*/
				std::string KernelString2();
				
				QString defaultValueToQString(double& value, int precision);
				QString defaultValueToQString(int& value);
				
				MainWindow* parent;


				/** @name Public Attributes*/
				bool entryHasKernel;
				bool entryHasParameters;
				bool isOptimizable;
				//bool params_optimized;
				//bool kernel_optimized;


	
			public slots:
				/** @name Public Slots */

				void changePage(QListWidgetItem *current, QListWidgetItem *previous);
				void applyModelParameters();
				void applyKernelParameters();
				void applyOptimizedParameters();
				void createModel();


			private:
				void createIcons();
				bool evaluateIndividualKernelFunction(String function);	

				QListWidget* contentsWidget;
				QStackedWidget* pagesWidget;
				
				QDialogButtonBox* buttons_;
				QPushButton* okButton_;

				ModelItem* model_item_;
				InputDataItem* input_;
				BALL::QSAR::RegistryEntry* entry_;

				string individual_kernel_string1_;
				string individual_kernel_string2_;
				
				ModelParameterPage* modelPage_;
				KernelParameterPage* kernelPage_;
				OptimizePage* optimizePage_;
				ModelPropertiesPage* propertyPage_;
				ConnectionsPage* connectionsPage_;
				DataPage* dataPage_;
		
				double parameter1_start_;
				double parameter2_start_;

				bool param1_is_set_;
				bool param2_is_set_;
		};
	}
}

#endif
