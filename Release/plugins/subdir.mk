################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../plugins/action.c \
../plugins/bind_mode.c \
../plugins/clock.c \
../plugins/cpu.c \
../plugins/external.c \
../plugins/image.c \
../plugins/memory.c \
../plugins/notification.c \
../plugins/text.c \
../plugins/window.c \
../plugins/workspace.c 

OBJS += \
./plugins/action.o \
./plugins/bind_mode.o \
./plugins/clock.o \
./plugins/cpu.o \
./plugins/external.o \
./plugins/image.o \
./plugins/memory.o \
./plugins/notification.o \
./plugins/text.o \
./plugins/window.o \
./plugins/workspace.o 

C_DEPS += \
./plugins/action.d \
./plugins/bind_mode.d \
./plugins/clock.d \
./plugins/cpu.d \
./plugins/external.d \
./plugins/image.d \
./plugins/memory.d \
./plugins/notification.d \
./plugins/text.d \
./plugins/window.d \
./plugins/workspace.d 


# Each subdirectory must supply rules for building sources it contributes
plugins/%.o: ../plugins/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../inc -O3 -Wall -Wextra -c -fmessage-length=0 `pkg-config --cflags gtk+-3.0` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


