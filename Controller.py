import cv2
import pygame
from threading import Thread
from queue import Queue, Empty
import requests

RTSP_URL = f"rtsp://Camera:Camera@192.168.0.100/stream2"
Server_IP = "192.168.126.174"

def refresh_video() -> pygame.Rect: # Blits onto display
    ret, frame = cap.read()
    if ret:
        frame_rgb = cv2.cvtColor(
            cv2.rotate(
                cv2.flip(
                    cv2.resize(frame, (640, 360), interpolation=cv2.INTER_AREA),
                    -1),
                cv2.ROTATE_90_CLOCKWISE),
            cv2.COLOR_BGR2RGB)
        pygame_surface = pygame.surfarray.make_surface(frame_rgb)
        return screen.blit(pygame_surface, (0, 0))


def update_joystick(pos: (int, int)) -> pygame.Rect:
    screen.fill((0, 0, 0), pygame.Rect(0, 360, 240, 240), )
    returnrect = screen.blit(joy, (5, 365))
    pygame.draw.circle(screen, (200, 200, 255), pos, 15)
    return returnrect


def update_asnake(pos: int) -> pygame.Rect:
    returnrect = screen.fill((0, 0, 0), pygame.Rect(242, 462, 395, 15), )
    screen.blit(snake, (243, 463))
    pygame.draw.circle(screen, (200, 200, 255), (pos, 468), 5)
    return returnrect


def update_ssnake(pos: int) -> pygame.Rect:
    returnrect = screen.fill((0, 0, 0), pygame.Rect(242, 562, 395, 15), )
    screen.blit(snake, (243, 563))
    pygame.draw.circle(screen, (200, 200, 255), (pos, 568), 5)
    return returnrect


def send_request(head: int, tail: int, yaw: bool, direction: str) -> int:
    left1 = left2 = right1 = right2 = 0
    if not direction:
        left1 = head
        if tail >= 0:
            right1 = tail
        else:
            right2 = -tail
    else:
        left2 = head
        if tail >= 0:
            right2 = tail
        else:
            right1 = -tail
    if yaw:
        left1, left2, right1, right2 = right1, right2, left1, left2
    print(
        str(left1).zfill(4) +
        str(left2).zfill(4) +
        str(right1).zfill(4) +
        str(right2).zfill(4)
    )


def solverequest(_amax: int, _smax: int, _apos: int, _spos: int) -> str:
    left1 = left2 = right1 = right2 = 0
    if _apos == 120 and _spos == 480:
        return (
            "/" +
            str(int(left1)).zfill(4) +
            str(int(left2)).zfill(4) +
            str(int(right1)).zfill(4) +
            str(int(right2)).zfill(4))
    yaw = _apos > 120
    direction = _spos > 480
    head = 4095 * (abs(_spos / 120 - 4)) * (_smax - 248) / 395
    tail = head - 4095 * (abs(_apos / 120 - 1)) * (_amax - 248) / 395
    left2 = right1 = right2 = 0
    left1 = head
    if tail > 0:
        right1 = tail
    else:
        right2 = -tail
    if yaw:
        left1, left2, right1, right2 = right1, right2, left1, left2
    if direction:
        left1, right1, left2, right2 = left2, right2, left1, right1
    return (
        "/" +
        str(int(left1)).zfill(4) +
        str(int(left2)).zfill(4) +
        str(int(right1)).zfill(4) +
        str(int(right2)).zfill(4)
    )
    return "/plForwa"


def initiatecap(_q: Queue):
    _q.put(cv2.VideoCapture(RTSP_URL))


def sendreq(c: str):
    try:
        requests.get("http://" + Server_IP + c)
        print("SUCCESS")
    except:
        print("FAILURE:" + c)


pygame.init()

joy = pygame.transform.scale(pygame.image.load("Joy.png"), (230, 230))
snake = pygame.transform.scale(pygame.image.load("Snake.png"), (387, 10))
q = Queue()
Thread(target=initiatecap, args=(q,)).start()
cap = None

t = Thread()
count = 0
command = ""
amax = 625
smax = 625
spos = 123
apos = 483
defjoypos = (123, 483)
defasnakepos = 625
defssnakepos = 625

key = None

pygame.display.set_caption("Controller")
screen = pygame.display.set_mode((640, 600))
pygame.display.update(update_joystick(defjoypos))
pygame.display.update(update_asnake(defasnakepos))
pygame.display.update(update_ssnake(defssnakepos))

while True:
    if not t.is_alive():
        t = Thread(target=sendreq, args=(command,))
        t.start()
        command = ""
    try:
        cap = q.get(block=False)
    except Empty:
        pass
    for event in pygame.event.get():
        match event.type:
            case pygame.QUIT:
                pygame.quit()
                exit()
            case pygame.MOUSEBUTTONDOWN if not key:
                pygame.display.update(update_joystick(pygame.mouse.get_pos()))
            case pygame.MOUSEMOTION if sum(pygame.mouse.get_pressed() and not key):
                pos = pygame.mouse.get_pos()
                if 0 < pos[0] < 240 and 360 < pos[1] < 600:
                    pygame.display.update(update_joystick(pos))
                    apos, spos = pos
                    command = solverequest(amax, smax, apos, spos)
                else:
                    pygame.display.update(update_joystick(defjoypos))
                    if 242 < pos[0] < 637 and 462 < pos[1] < 477:
                        amax = max(249, min(pos[0], 625))
                        pygame.display.update(update_asnake(amax))
                    elif 242 < pos[0] < 637 and 562 < pos[1] < 577:
                        smax = max(249, min(pos[0], 625))
                        pygame.display.update(update_ssnake(smax))
            case pygame.MOUSEBUTTONUP if not key:
                pygame.display.update(update_joystick(defjoypos))
                apos = 120
                spos = 480
                command = solverequest(amax, smax, apos, spos)
            case pygame.KEYDOWN if not key:
                key = event.key
                match event.key:
                    case pygame.K_w:
                        command = str(smax).zfill(4) + 'f'
                    case pygame.K_s:
                        command = str(smax).zfill(4) + 'b'
                    case pygame.K_a:
                        command = str(smax).zfill(4) + 'l'
                    case pygame.K_d:
                        command = str(smax).zfill(4) + 'r'
            case pygame.KEYUP if key == event.key:
                key = None
                command = "0000" + command[-1]
    print(command)
    if cap:
        pygame.display.update(refresh_video())
    screen.fill((0,0,0), pygame.Rect(350,510,350,55))
    textrect = screen.blit(font.render(
        str(apos) + "/" +
        str(amax - 249) + " " +
        str(spos - 360) + "/" +
        str(smax - 249)
        , False, (100,100,255)), (350,510))
    pygame.display.update(textrect)
