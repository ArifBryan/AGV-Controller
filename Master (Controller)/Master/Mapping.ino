#define DRIVE_STOP        0
#define DRIVE_NORTH       1
#define DRIVE_NORTHWEST   2
#define DRIVE_NORTHEAST   3
#define DRIVE_WEST        4
#define DRIVE_WESTNORTH   5
#define DRIVE_WESTSOUTH   6
#define DRIVE_SOUTH       7
#define DRIVE_SOUTHWEST   8
#define DRIVE_SOUTHEAST   9
#define DRIVE_EAST        10
#define DRIVE_EASTNORTH   11
#define DRIVE_EASTSOUTH   12

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

#define RFID_TAG_COUNT  4
#define PATH_COUNT      3

bool map_StopAtEndpoint;
uint8_t map_Position;

uint8_t map_Destination;

uint8_t map_JunctionTag[RFID_TAG_COUNT] = {1, 2, 3, 4};

uint8_t map_JunctionPos[RFID_TAG_COUNT][2] = {
  {map_JunctionTag[0], TAG_LON},
  {map_JunctionTag[1], TAG_LAT},
  {map_JunctionTag[2], TAG_LON},
  {map_JunctionTag[3], TAG_LAT},
};

uint8_t map_JuctionTag_Path[PATH_COUNT][RFID_TAG_COUNT] = {
// Tag1             Tag2          Tag3          Tag4
  {DRIVE_SOUTH, DRIVE_SOUTHWEST, DRIVE_SOUTH, DRIVE_SOUTHEAST}, // Parking
  {DRIVE_NORTHWEST, DRIVE_WEST, DRIVE_SOUTHWEST, DRIVE_WEST},   // Room 1
  {DRIVE_NORTHEAST, DRIVE_EAST, DRIVE_SOUTHEAST, DRIVE_EAST},   // Room 2
};

uint8_t map_DestinationTagHeading[PATH_COUNT][2] = {
  {map_JunctionTag[0], HEADING_SOUTH},
  {map_JunctionTag[3], HEADING_WEST},
  {map_JunctionTag[1], HEADING_EAST}
};

void Mapping_Destination(uint8_t dest){
  map_Destination = dest;
  if(dest != 0){
//    uint8_t headPos = map_DestinationTagHeading[map_Position][2];
    uint8_t headPos = Motion_GetHeading();
    switch(headPos){
      case HEADING_NORTH:
        Motion_Drive(DRIVE_NORTH);
        map_StopAtEndpoint = false;
      break;
      case HEADING_WEST:
        Motion_Drive(DRIVE_WEST);
        map_StopAtEndpoint = false;
      break;
      case HEADING_SOUTH:
        Motion_Drive(DRIVE_SOUTH);
        map_StopAtEndpoint = false;
      break;
      case HEADING_EAST:
        Motion_Drive(DRIVE_EAST);
        map_StopAtEndpoint = false;
      break;
      case HEADING_UNKNOWN:
        if(LineSensor_NumDetected(Motion_GetHead()) >= 15 || map_StopAtEndpoint){
          Motion_Drive(DRIVE_SOUTH);
        }
        else{
          Motion_Drive(DRIVE_NORTH);
        }
      break;
    }
  }
}

void Mapping_Handler(){
  if(map_StopAtEndpoint){
    if(LineSensor_NumDetected(Motion_GetHead()) >= 15){
      Motion_Drive(DRIVE_STOP);
      switch(Motion_GetHeading()){
      case HEADING_NORTH:
        Motion_SetHeading(HEADING_SOUTH);
        _HeadFlip();
        map_StopAtEndpoint = false;
      break;
      case HEADING_WEST:
        Motion_SetHeading(HEADING_EAST);
        _HeadFlip();
        map_StopAtEndpoint = false;
      break;
      case HEADING_SOUTH:
        Motion_SetHeading(HEADING_NORTH);
        _HeadFlip();
        map_StopAtEndpoint = false;
      break;
      case HEADING_EAST:
        Motion_SetHeading(HEADING_WEST);
        _HeadFlip();
        map_StopAtEndpoint = false;
      break;
    }
    }
  }
}

void Mapping_RFIDHandler(uint8_t pcdNum, uint8_t uid[4]){
  // Tag number lookup.
  uint8_t tagNum = Tag_Lookup(uid);
  if(tagNum == 0) return;
  
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
    Motion_Drive(map_JuctionTag_Path[map_Destination - 1][tagNum - 1]);
    if(map_DestinationTagHeading[map_Destination - 1][0] == tagNum){
      if(map_DestinationTagHeading[map_Destination - 1][1] == Motion_GetHeading()){
        map_StopAtEndpoint = true;
      }
      else{
        map_StopAtEndpoint = false;
      }
    }
  }
}
