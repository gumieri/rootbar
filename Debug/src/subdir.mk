################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bar.c \
../src/config.c \
../src/main.c \
../src/map.c \
../src/sway_ipc.c \
../src/utils.c 

OBJS += \
./src/bar.o \
./src/config.o \
./src/main.o \
./src/map.o \
./src/sway_ipc.o \
./src/utils.o 

C_DEPS += \
./src/bar.d \
./src/config.d \
./src/main.d \
./src/map.d \
./src/sway_ipc.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../inc -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -fsanitize=address `pkg-config --cflags gtk+-3.0` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


