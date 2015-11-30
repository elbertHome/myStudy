################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Configuration.cpp \
../commontools.cpp \
../sim_nsim_main.cpp 

O_SRCS += \
../Configuration.o \
../commontools.o 

OBJS += \
./Configuration.o \
./commontools.o \
./sim_nsim_main.o 

CPP_DEPS += \
./Configuration.d \
./commontools.d \
./sim_nsim_main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


