# ZakuHead

Arduino script for 3D printed model of Zaku Head.

### Pin Assignment:
| Pin | Usage |
| ------ | ------ |
| GPIO-9 | Connect to servo data pin |
| GPIO-10 | Connect to LED |
| GPIO-11 | Connect to touch pad data pin |

### Default Action:  
- Patrol in every 30 minutes

### Default Reponse on Touch Sensor (detected within 1.5s):
| Touch | Response |
| ------ | ------ |
| Single | Patrol |
| Double | Turn LED On |
| Triple | Turn LED Off |
| Hold | Alert : LED flash |
