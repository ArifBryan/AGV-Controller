#define SLAVE1ID 0x01
#define SLAVE2ID 0x02
#define SLAVE3ID 0x03

#define DRIVE_STOP        0
#define DRIVE_NORTH       1
#define DRIVE_NORTHWEST   2
#define DRIVE_NORTHEAST   3
#define DRIVE_WEST        4
#define DRIVE_SOUTH       5
#define DRIVE_SOUTHWEST   6
#define DRIVE_SOUTHEAST   7
#define DRIVE_EAST        8

#define HEAD_FRONT  0
#define HEAD_REAR   1

#define HEADING_UNKNOWN 0
#define HEADING_NORTH   1
#define HEADING_WEST    2
#define HEADING_SOUTH   3
#define HEADING_EAST    4

#define TAG_LAT 0
#define TAG_LON 1

#define PCD_RIGHT 0
#define PCD_LEFT  1

#define RFID_TAG_COUNT  7
#define PATH_COUNT      4

bool map_StopAtEndpoint;
uint8_t map_ArrivedPosition;
uint8_t map_Position;
uint8_t map_Destination;

uint8_t map_JunctionTag[RFID_TAG_COUNT] = {1, 2, 3, 4, 5, 6, 7};

uint8_t map_JunctionPos[RFID_TAG_COUNT][2] = {
  {map_JunctionTag[0], TAG_LON},
  {map_JunctionTag[1], TAG_LAT},
  {map_JunctionTag[2], TAG_LON},
  {map_JunctionTag[3], TAG_LAT},
  {map_JunctionTag[4], TAG_LON},
  {map_JunctionTag[5], TAG_LON},
  {map_JunctionTag[6], TAG_LAT},
};

uint8_t map_JunctionPath[PATH_COUNT][RFID_TAG_COUNT] = {
// J0               J1               J2               J3               J4               J5               J6
  {DRIVE_SOUTH,     DRIVE_SOUTHWEST, DRIVE_SOUTH,     DRIVE_SOUTHEAST, DRIVE_SOUTH,     DRIVE_SOUTH,     DRIVE_SOUTHEAST},   // Parking
  {DRIVE_NORTHWEST, DRIVE_WEST,      DRIVE_SOUTHWEST, DRIVE_WEST,      DRIVE_SOUTH,     DRIVE_SOUTH,     DRIVE_SOUTHEAST},   // Room 1
  {DRIVE_NORTHEAST, DRIVE_EAST,      DRIVE_SOUTHEAST, DRIVE_EAST,      DRIVE_SOUTH,     DRIVE_SOUTH,     DRIVE_SOUTHEAST},   // Room 2
  {DRIVE_NORTH,     DRIVE_NORTHWEST, DRIVE_NORTH,     DRIVE_NORTHEAST, DRIVE_NORTHWEST, DRIVE_SOUTHWEST, DRIVE_WEST     },   // Room 3
};

uint8_t map_JunctionEndpoint[PATH_COUNT][2] = {
  {map_JunctionTag[0], HEADING_SOUTH},  // Parking
  {map_JunctionTag[3], HEADING_WEST},   // Room 1
  {map_JunctionTag[1], HEADING_EAST},    // Room 2
  {map_JunctionTag[6], HEADING_WEST},    // Room 3
};

uint8_t Mapping_GetPosition(){
  if(Motion_GetHeading() == HEADING_UNKNOWN){
    return 0;
  }
  else{
    return map_Position + 1;
  }
}

uint8_t Mapping_ArrivedPosition(){
  return map_ArrivedPosition;
}

uint8_t Mapping_GetDestination(){
  return map_Destination;
}

void Mapping_Destination(uint8_t dest){
  if(dest == Mapping_ArrivedPosition() || dest > 4) return;
  map_Destination = dest;
  if(dest != 0){
    map_ArrivedPosition = 0;
    uint8_t headPos = Motion_GetHeading();
    switch(headPos){
      case HEADING_NORTH:
        Motion_Drive(DRIVE_SOUTH);
        map_StopAtEndpoint = false;
      break;
      case HEADING_WEST:
        Motion_Drive(DRIVE_EAST);
        map_StopAtEndpoint = false;
      break;
      case HEADING_SOUTH:
        Motion_Drive(DRIVE_NORTH);
        map_StopAtEndpoint = false;
      break;
      case HEADING_EAST:
        Motion_Drive(DRIVE_WEST);
        map_StopAtEndpoint = false;
      break;
      case HEADING_UNKNOWN:
        if(LineSensor_NumDetected(Motion_GetHead()) >= 13 || map_StopAtEndpoint){
          Motion_Drive(DRIVE_SOUTH);
          map_StopAtEndpoint = false;
        }
        else{
          Motion_Drive(DRIVE_NORTH);
          map_StopAtEndpoint = true;
        }
      break;
    }
  }
}

void Mapping_Handler(){
  if(map_StopAtEndpoint){
    if(LineSensor_NumDetected(Motion_GetHead()) >= 13){
      if(Motion_GetHeading() == HEADING_UNKNOWN){
        _HeadFlip();
        PID_Reset();
        Slave_Init(SLAVE2ID);
        Slave_Init(SLAVE3ID);
      }
      else{
        map_ArrivedPosition = map_Destination;
        map_StopAtEndpoint = false;
        Motion_Drive(DRIVE_STOP);
        UserInterface_Beep(500);
      }
    }
  }
}

void Mapping_RFIDHandler(uint8_t pcdNum, uint8_t uid[4]){
  // Tag number lookup.
  uint8_t tagNum = Tag_Lookup(uid);
  if(tagNum == 0 || Motion_GetDrive() == DRIVE_STOP) return;
  
  // Heading detection.
  for(uint8_t i = 0; i < RFID_TAG_COUNT; i++){
    if(tagNum == map_JunctionPos[i][0]){
      if(map_JunctionPos[i][1] == TAG_LON){
        if(Motion_GetHead() == HEAD_FRONT){
          if(pcdNum == PCD_RIGHT){
            Motion_SetHeading(HEADING_NORTH);
          }
          else if(pcdNum == PCD_LEFT){
            Motion_SetHeading(HEADING_SOUTH);
          }
        }
        else if(Motion_GetHead() == HEAD_REAR){
          if(pcdNum == PCD_RIGHT){
            Motion_SetHeading(HEADING_SOUTH);
          }
          else if(pcdNum == PCD_LEFT){
            Motion_SetHeading(HEADING_NORTH);
          }
        }
      }
      else if(map_JunctionPos[i][1] == TAG_LAT){
        if(Motion_GetHead() == HEAD_FRONT){
          if(pcdNum == PCD_RIGHT){
            Motion_SetHeading(HEADING_WEST);
          }
          else if(pcdNum == PCD_LEFT){
            Motion_SetHeading(HEADING_EAST);
          }
        }
        else if(Motion_GetHead() == HEAD_REAR){
          if(pcdNum == PCD_RIGHT){
            Motion_SetHeading(HEADING_EAST);
          }
          else if(pcdNum == PCD_LEFT){
            Motion_SetHeading(HEADING_WEST);
          }
        }
      }
      map_Position = i;
      break;
    }
  }
  
  // Path control.
  if(map_Destination != 0){
    uint8_t junction = 0;
    for(uint8_t i = 0; i < RFID_TAG_COUNT; i ++){
      if(map_JunctionTag[i] == tagNum){
        junction = i;
        break;
      }
    }
    Motion_Drive(map_JunctionPath[map_Destination - 1][junction]);
    if(map_JunctionEndpoint[map_Destination - 1][0] == tagNum){
      map_StopAtEndpoint = true;
    }
    else{
      map_StopAtEndpoint = false;
    }
  }
}
