#ifndef SPACENAVIGATORDRIVER_H
#define SPACENAVIGATORDRIVER_H

#include <BALL/VIEW/INPUT/inputDeviceDriver.h>

#include <QtCore/QThread>

namespace BALL
{
	namespace VIEW
	{

		class SpaceNavigatorDriver : public InputDeviceDriver, protected QThread
		{
			public:
				SpaceNavigatorDriver(QWidget* receiver);

				bool setUp();
				bool tearDown();

				void setEnabled(bool enabled);

			private:
				void run();

				int deadzone(int x);
		};

	}
}

#endif //SPACENAVIGATORDRIVER_H
