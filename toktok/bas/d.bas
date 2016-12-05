'Set default data type to integer for faster game play
DEFINT A-Z

'Sub Declarations
DECLARE SUB RestReal (t#)
DECLARE SUB AlertSnd ()
DECLARE SUB LoadSettings ()
DECLARE SUB Center (Row, Text$)
DECLARE SUB DoBeep ()
DECLARE SUB DoExplosion (x#, y#)
DECLARE SUB DoSun (Mouth)
DECLARE SUB DrawBan (xc#, yc#, r, bc)
DECLARE SUB DrawGorilla (x, y, arms)
DECLARE SUB ExplodeGorilla (x#, y#, PlayerHit)
DECLARE SUB Extro ()
DECLARE SUB GetInputs (Player$(), NumGames, P)
DECLARE SUB GorillaIntro (Player$(), cIntro)
DECLARE SUB Intro ()
DECLARE SUB MakeCityScape (BCoor() AS ANY)
DECLARE SUB PlaceGorillas (BCoor() AS ANY)
DECLARE SUB Rest (t#)
DECLARE SUB SetScreen ()
DECLARE SUB ShowPrompts (fieldNum AS INTEGER)
DECLARE SUB Slidy ()
DECLARE SUB SparklePause (opt AS INTEGER)
DECLARE SUB Stats (Wins(), name$(), Ban!(), P, abortYN)
DECLARE SUB VictoryDance (Player)

DECLARE FUNCTION CalcDelay# ()
DECLARE FUNCTION DoShot (Player$(), PlayerNum, x, y, turn, othX, othY)
DECLARE FUNCTION Get$ (Row, Col, Prev$, Typ, Max, Esc)
DECLARE FUNCTION PlayGame (Player$(), NumGames, P)
DECLARE FUNCTION PlotShot (StartX, StartY, angle#, velocity, PlayerNum, othX, othY)
DECLARE FUNCTION Scl (N!)
DECLARE FUNCTION WhereX (num)
DECLARE FUNCTION WhereY (num)

'Make all arrays Dynamic
'$DYNAMIC

' User-Defined TYPEs

TYPE settings
  useSound AS INTEGER
  useOldExplosions AS INTEGER
  newExplosionRadius AS INTEGER
  useSlidingText AS INTEGER
  defaultGravity AS INTEGER
  defaultRoundQty AS INTEGER
  showIntro AS INTEGER
  forceCGA AS INTEGER
END TYPE

a$ = "hello world"