import cv2
import pygame
from threading import Thread
from queue import Queue, Empty
import requests
from time import sleep

RTSP_URL = f"rtsp://camera:camera@192.168.40.93/stream2"
Server_IP = "192.168.40.246"

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


def solverequest(_amax: int, _smax: int, _apos: int, _spos: int) -> str:
    left1 = left2 = right1 = right2 = 0
    if (_apos, _spos) == defjoypos:
        return (
            "/" +
            str(int(left1)).zfill(4) +
            str(int(left2)).zfill(4) +
            str(int(right1)).zfill(4) +
            str(int(right2)).zfill(4))
    yaw = _apos > defjoypos[0]
    direction = _spos > defjoypos[1]
    head = 4095 * (abs(((_spos - 360) / (defjoypos[1] - 360)) - 1)) * (_smax - 248) / 395
    tail = head - 4095 * (abs((_apos / defjoypos[0]) - 1)) * (_amax - 248) / 395
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


def initiatecap(_q: Queue):
    _q.put(cv2.VideoCapture(RTSP_URL))


def sendreq(c: str):
    try:
        requests.get("http://" + Server_IP + c)
    except requests.exceptions.HTTPError:
        print("FAILURE:" + c)


# Settles video feed
q = Queue()
cap = None
Thread(target=initiatecap, args=(q,)).start()

# Settles http requests
t = Thread()
command = ""
heading = ''
key = None
mode = True

pygame.init()
joy = pygame.transform.scale(pygame.image.load("Joy.png"), (230, 230))
snake = pygame.transform.scale(pygame.image.load("Snake.png"), (387, 10))
blitlist = []
defasnakepos = amax = 625
defssnakepos = smax = 625
defjoypos = (123, 483)
apos, spos = defjoypos

pygame.display.set_caption("Controller")
screen = pygame.display.set_mode((640, 600))
pygame.display.update(update_joystick(defjoypos))
pygame.display.update(update_asnake(defasnakepos))
pygame.display.update(update_ssnake(defssnakepos))
while True:
    for event in pygame.event.get():
        match event.type:
            case pygame.QUIT:
                pygame.quit()
                exit()
            case pygame.KEYDOWN if event.key == pygame.K_SPACE:
                mode = not mode
                break
                blitlist.append(update_joystick(defjoypos))
            case pygame.KEYDOWN if event.key == pygame.K_g:
                sendreq("/Z")
                sleep(5)
                break
            case _:
                if mode:
                    match event.type:
                        case pygame.KEYDOWN if not key:
                            key = event.key
                            match event.key:
                                case pygame.K_w:
                                    heading = 'f'
                                case pygame.K_s:
                                    heading = 'b'
                                case pygame.K_a:
                                    heading = 'l'
                                case pygame.K_d:
                                    heading = 'r'
                            command = "/A" + str(int(4095 * (smax - 248) / 377)).zfill(4) + heading
                        case pygame.KEYUP if key == event.key and (len(command) == 7 or len(command) == 17):
                            key = None
                            command = "/A0000" + heading
                        case pygame.MOUSEBUTTONDOWN:
                            mpos = pygame.mouse.get_pos()
                            if 242 < mpos[0] < 637 and 562 < mpos[1] < 577:
                                smax = max(249, min(mpos[0], 625))
                                blitlist.append(update_ssnake(smax))
                        case pygame.MOUSEMOTION if sum(pygame.mouse.get_pressed()):
                            mpos = pygame.mouse.get_pos()
                            if 242 < mpos[0] < 637 and 562 < mpos[1] < 577:
                                smax = max(249, min(mpos[0], 625))
                                blitlist.append(update_ssnake(smax))
                else:
                    match event.type:
                        case pygame.MOUSEBUTTONDOWN:
                            blitlist.append(update_joystick(pygame.mouse.get_pos()))
                        case pygame.MOUSEMOTION if sum(pygame.mouse.get_pressed()):
                            pos = pygame.mouse.get_pos()
                            if 0 < pos[0] < 240 and 360 < pos[1] < 600:
                                blitlist.append(update_joystick(pos))
                                apos, spos = pos
                                command = solverequest(amax, smax, apos, spos)
                            else:
                                blitlist.append(update_joystick(defjoypos))
                                if 242 < pos[0] < 637 and 462 < pos[1] < 477:
                                    amax = max(249, min(pos[0], 625))
                                    blitlist.append(update_asnake(amax))
                                elif 242 < pos[0] < 637 and 562 < pos[1] < 577:
                                    smax = max(249, min(pos[0], 625))
                                    blitlist.append(update_ssnake(smax))
                        case pygame.MOUSEBUTTONUP:
                            blitlist.append(update_joystick(defjoypos))
                            apos, spos = defjoypos
                            command = solverequest(amax, smax, apos, spos)
    if command:
        print("\r" + str(len(command)) + command, end="")
        if not t.is_alive():
            t = Thread(target=sendreq, args=(command,))
            t.start()
    if not cap:
        try:
            cap = q.get(block=False)
        except Empty:
            pass
    else:
        blitlist.append(refresh_video())
    pygame.display.update(blitlist)
    blitlist = []
