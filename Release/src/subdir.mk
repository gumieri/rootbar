################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bar.c \
../src/config.c \
../src/main.c \
../src/map.c 

OBJS += \
./src/bar.o \
./src/config.o \
./src/main.o \
./src/map.o 

C_DEPS += \
./src/bar.d \
./src/config.d \
./src/main.d \
./src/map.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I../inc -I/usr/include/cairo -I/usr/include/atk-1.0 -I/usr/include/gtk-3.0 -I/usr/include/glib-2.0 -I/usr/include/pango-1.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


