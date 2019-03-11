################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/synergy_gen/audio_playback_thread.c \
../src/synergy_gen/common_data.c \
../src/synergy_gen/hal_data.c \
../src/synergy_gen/main.c \
../src/synergy_gen/message_data.c \
../src/synergy_gen/pin_data.c \
../src/synergy_gen/pwm_control.c \
../src/synergy_gen/system_thread.c \
../src/synergy_gen/temperature_thread.c 

OBJS += \
./src/synergy_gen/audio_playback_thread.o \
./src/synergy_gen/common_data.o \
./src/synergy_gen/hal_data.o \
./src/synergy_gen/main.o \
./src/synergy_gen/message_data.o \
./src/synergy_gen/pin_data.o \
./src/synergy_gen/pwm_control.o \
./src/synergy_gen/system_thread.o \
./src/synergy_gen/temperature_thread.o 

C_DEPS += \
./src/synergy_gen/audio_playback_thread.d \
./src/synergy_gen/common_data.d \
./src/synergy_gen/hal_data.d \
./src/synergy_gen/main.d \
./src/synergy_gen/message_data.d \
./src/synergy_gen/pin_data.d \
./src/synergy_gen/pwm_control.d \
./src/synergy_gen/system_thread.d \
./src/synergy_gen/temperature_thread.d 


# Each subdirectory must supply rules for building sources it contributes
src/synergy_gen/%.o: ../src/synergy_gen/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v6.2.1_ssp_v1.5.0\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DSF_MESSAGE_CLASS -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\driver" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\bsp" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\driver\api" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\driver\instances" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\src" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\src\synergy_gen" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\framework" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\api" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\instances" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\tes" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\el" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\ssp\src\framework\el\tx" -I"C:\Users\JGFMEXID\e2_studio\workspace\Integrator_Project_SK_S7G2\synergy\board\s7g2_sk" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


