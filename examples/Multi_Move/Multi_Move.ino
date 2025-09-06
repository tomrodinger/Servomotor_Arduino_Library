#include "Servomotor.h"

// Create Servomotor instance on the stack
// This is safe because ArduinoEmulator.cpp ensures Serial1 is initialized
// before setup() is called, and exits if initialization fails
Servomotor motor('X', Serial1);

void setup() {
    Serial.begin(115200);  // Debugging port at 115200 baud

    // Configure motor units
    motor.setPositionUnit(PositionUnit::SHAFT_ROTATIONS);
    motor.setVelocityUnit(VelocityUnit::ROTATIONS_PER_SECOND);
    motor.setAccelerationUnit(AccelerationUnit::ROTATIONS_PER_SECOND_SQUARED);
    motor.setTimeUnit(TimeUnit::SECONDS);

    // Enable mosfets before moving
    motor.enableMosfets();

    // Get initial position
    float start_pos = motor.getPosition();
    Serial.print("Position before move: ");
    Serial.print(start_pos, 2);
    Serial.println(" rotations");

    // Create a sequence of moves
    const uint8_t multiMoveCount = 4;
    uint32_t multiMoveTypes = 0b1001;  // 1001 in binary: 1st and 4th are velocity moves, 2nd and 3rd are acceleration moves

    // Create move list with user units
    multimoveListConverted_t multiMoveList[multiMoveCount] = {
        {2.0f, 1.0f},    // Velocity move: 2 rot/sec for 1 sec
        {2.0f, 1.0f},    // Acceleration move: 2 rot/sec² for 1 sec
        {-2.0f, 1.0f},   // Acceleration move: -2 rot/sec² for 1 sec
        {0.0f, 0.1f}     // Velocity move: 0 rot/sec for 0.1 sec (stop)
    };

    // Execute the multi-move sequence with automatic unit conversion
    Serial.println("Executing multi-move sequence...");

    motor.multimove(multiMoveCount, multiMoveTypes, multiMoveList);

    // Calculate total duration of the sequence
    float totalDuration = 0.0f;
    for (int i = 0; i < multiMoveCount; i++) {
        totalDuration += multiMoveList[i].duration;
    }

    // Add a buffer of 5% to ensure the sequence completes
    float waitTime = totalDuration * 1.05f;
    Serial.print("Waiting for ");
    Serial.print(waitTime, 2);
    Serial.print(" seconds (sequence duration: ");
    Serial.print(totalDuration, 2);
    Serial.println(" seconds + 5% buffer)");

    // Wait for the sequence to complete
    delay(static_cast<int>(waitTime * 1000));

    // Get final position
    float end_pos = motor.getPosition();
    Serial.print("Position after move: ");
    Serial.print(end_pos, 2);
    Serial.println(" rotations");

    // Expected position: 8 rotations
    Serial.print("Expected position: ");
    Serial.print(start_pos + 8.0f, 2);
    Serial.println(" rotations");
}

void loop() {
    // Not used
}