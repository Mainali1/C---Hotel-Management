# Hotel Management System

## Overview
This is a comprehensive hotel management system implemented in C. The system is designed to handle various aspects of hotel operations including room management, guest information, reservations, billing, and reporting.

## Features
- **User Authentication**: Secure login system for staff members with different access levels
- **Room Management**: Add, modify, and view room details including type, status, and pricing
- **Guest Management**: Register guests, store and retrieve guest information
- **Reservation System**: Book rooms, modify bookings, and check availability
- **Billing System**: Generate bills, process payments, and maintain financial records
- **Reporting**: Generate various reports for management decision-making
- **Data Persistence**: All data is stored in files with proper backup mechanisms

## Technical Details
- Implemented entirely in C language
- Uses file handling for data storage and retrieval
- Implements data structures for efficient information management
- Includes error handling and data validation
- Features a user-friendly text-based interface

## Project Structure
- `main.c`: Entry point of the application
- `auth.c/h`: User authentication functionality
- `room.c/h`: Room management functionality
- `guest.c/h`: Guest information management
- `reservation.c/h`: Reservation system
- `billing.c/h`: Billing and payment processing
- `fileio.c/h`: File I/O operations for data persistence
- `utils.c/h`: Utility functions
- `ui.c/h`: User interface functions
- `data/`: Directory for data files

## Compilation and Execution
```bash
gcc -o hotel_management main.c auth.c room.c guest.c reservation.c billing.c fileio.c utils.c ui.c
./hotel_management
```

## Academic Purpose
This project is developed for academic purposes, demonstrating the application of C programming concepts in creating a robust, real-world application with proper documentation and implementation of best practices.