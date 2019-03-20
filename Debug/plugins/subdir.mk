################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../plugins/bind_mode.c \
../plugins/clock.c \
../plugins/cpu.c \
../plugins/text.c \
../plugins/window.c \
../plugins/workspace.c 

OBJS += \
./plugins/bind_mode.o \
./plugins/clock.o \
./plugins/cpu.o \
./plugins/text.o \
./plugins/window.o \
./plugins/workspace.o 

C_DEPS += \
./plugins/bind_mode.d \
./plugins/clock.d \
./plugins/cpu.d \
./plugins/text.d \
./plugins/window.d \
./plugins/workspace.d 


# Each subdirectory must supply rules for building sources it contributes
plugins/%.o: ../plugins/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../inc -I/usr/include/cairo -I/usr/include/atk-1.0 -I/usr/include/gtk-3.0 -I/usr/include/glib-2.0 -I/usr/include/pango-1.0 -I/usr/lib/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -fsanitize=address -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


