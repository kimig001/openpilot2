import math

from common.kf import KalmanMean
from selfdrive.controls.lib.pid import PIDController
from selfdrive.controls.lib.drive_helpers import get_steer_max
from selfdrive.controls.lib.latcontrol import LatControl, MIN_STEER_SPEED
from cereal import log


class LatControlPID(LatControl):
  def __init__(self, CP, CI):
    super().__init__(CP)
    self.pid = PIDController ((CP.lateralTuning.pid.kpBP, CP.lateralTuning.pid.kpV),
                              (CP.lateralTuning.pid.kiBP, CP.lateralTuning.pid.kiV),
                              (CP.lateralTuning.pid.kdBP, CP.lateralTuning.pid.kdV),
                              k_f=CP.lateralTuning.pid.kf, pos_limit=1.0, neg_limit=-1.0,
                              derivative_period=0.1)
    self.new_kf_tuned = CP.lateralTuning.pid.newKfTuned
    self.get_steer_feedforward = CI.get_steer_feedforward
    self.kf_mean = KalmanMean()

  def reset(self):
    super().reset()
    self.pid.reset()

  def update(self, active, CS, CP, VM, params, desired_curvature, desired_curvature_rate, roll):
    pid_log = log.ControlsState.LateralPIDState.new_message()
    pid_log.steeringAngleDeg = float(CS.steeringAngleDeg)
    pid_log.steeringRateDeg = float(CS.steeringRateDeg)

    angle_steers_des_no_offset = math.degrees(VM.get_steer_from_curvature(-desired_curvature, CS.vEgo, roll))
    angle_steers_des = angle_steers_des_no_offset + params.angleOffsetDeg

    pid_log.angleError = angle_steers_des - CS.steeringAngleDeg
    if CS.vEgo < MIN_STEER_SPEED or not active:
      output_steer = 0.0
      pid_log.active = False
      self.pid.reset()
    else:
      steers_max = get_steer_max(CP, CS.vEgo)
      self.pid.pos_limit = steers_max
      self.pid.neg_limit = -steers_max

      # offset does not contribute to resistive torque
      steer_feedforward = self.get_steer_feedforward(angle_steers_des_no_offset, CS.vEgo)

      deadzone = 0.0

      output_steer = self.pid.update(angle_steers_des, CS.steeringAngleDeg, override=CS.steeringPressed,
                                     feedforward=steer_feedforward, speed=CS.vEgo, deadzone=deadzone)
      
      # estimates average kf using accurate feedforward function, only when actively steering
      if abs(CS.steeringRateDeg) < 10 and abs(angle_steers_des_no_offset) > 10 and CS.vEgo > 5:
        self.kf_mean.predict(abs(output_steer / (angle_steers_des_no_offset * steer_feedforward)))
        self.pid.k_f = self.kf_mean.x_hat
        print('Using kf: {}'.format(round(self.kf_mean.x_hat, 7)))
        
      pid_log.active = True
      pid_log.p = self.pid.p
      pid_log.i = self.pid.i
      pid_log.f = self.pid.f
      pid_log.output = output_steer
      pid_log.saturated = self._check_saturation(output_steer, steers_max, CS)

    return output_steer, angle_steers_des, pid_log
