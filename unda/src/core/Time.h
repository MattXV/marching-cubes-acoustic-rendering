#pragma once


namespace unda {
	// TODO: Implement integration
	class Time {
	public:
		inline static double getTime() { return singleton->getTimeImplementation(); }
		inline static void setDeltaTime(const double& newDeltaTime) { singleton->setDeltaTimeImplementation(newDeltaTime); }
		inline static const double& getDeltaTime() { return singleton->getDeltaTimeImplementation(); }
		
		static void setInstance(Time* newInstance) { singleton = newInstance; }
	protected:
		virtual double getTimeImplementation() = 0;
		virtual const double& getDeltaTimeImplementation() = 0;
		virtual void setDeltaTimeImplementation(const double& newDeltaTime) = 0;
	private:
		static Time* singleton;
	};
}