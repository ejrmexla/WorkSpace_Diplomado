################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/audio.c \
../src/audio_playback_thread_entry.c \
../src/audio_sample.c \
../src/hal_entry.c \
../src/pwm_control_entry.c \
../src/system_thread_entry.c \
../src/system_time.c \
../src/temperature_thread_entry.c 

OBJS += \
./src/audio.o \
./src/audio_playback_thread_entry.o \
./src/audio_sample.o \
./src/hal_entry.o \
./src/pwm_control_entry.o \
./src/system_thread_entry.o \
./src/system_time.o \
./src/temperature_thread_entry.o 

C_DEPS += \
./src/audio.d \
./src/audio_playback_thread_entry.d \
./src/audio_sample.d \
./src/hal_entry.d \
./src/pwm_control_entry.d \
./src/system_thread_entry.d \
./src/system_time.d \
./src/temperature_thread_entry.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v6.2.1_ssp_v1.5.0\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DSF_MESSAGE_CLASS -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\driver" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\bsp" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\driver\api" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\driver\instances" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\src" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\src\synergy_gen" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\framework" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\api" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\instances" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\tes" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\inc\framework\el" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\ssp\src\framework\el\tx" -I"C:\Users\JGFMEXID\Documents\WorkSpace_DSE\WorkSpace_Diplomado\Integrator_Project_SK_S7G2\synergy\board\s7g2_sk" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


