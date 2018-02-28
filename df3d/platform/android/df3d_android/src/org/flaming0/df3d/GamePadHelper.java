package org.flaming0.df3d;

import android.content.Context;
import android.hardware.input.InputManager;
import android.util.SparseArray;
import android.util.SparseIntArray;
import android.view.InputDevice;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import java.util.List;

class GamePadHelper implements InputManager.InputDeviceListener {
    private static final String TAG = "GamePadHelper";

    private InputManager mInputManager;
    private SparseArray<InputDeviceState> mInputDeviceStates;
    private Df3dActivity mActivity;

    private float mOldLeftThumbstickX = 0.0f;
    private float mOldLeftThumbstickY = 0.0f;
    private float mOldRightThumbstickX = 0.0f;
    private float mOldRightThumbstickY = 0.0f;

    private native void nativeControllerConnected(int deviceId);
    private native void nativeControllerDisconnected(int deviceId);

    private native void nativeControllerButtonPressedA(boolean pressed);
    private native void nativeControllerButtonPressedX(boolean pressed);
    private native void nativeControllerButtonPressedY(boolean pressed);
    private native void nativeControllerButtonPressedB(boolean pressed);
    private native void nativeControllerButtonPressedMenu(boolean pressed);

    private native void nativeControllerButtonPressedDPadLeft(boolean pressed);
    private native void nativeControllerButtonPressedDPadRight(boolean pressed);
    private native void nativeControllerButtonPressedDPadUp(boolean pressed);
    private native void nativeControllerButtonPressedDPadDown(boolean pressed);

    private native void nativeControllerButtonPressedR1(boolean pressed);
    private native void nativeControllerButtonPressedR2(boolean pressed);
    private native void nativeControllerButtonPressedL1(boolean pressed);
    private native void nativeControllerButtonPressedL2(boolean pressed);

    private native void nativeControllerThumbStickEvent(boolean isLeft, float x, float y);

    private boolean deviceIsJoystick(InputDevice device) {
        return (device.getSources() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK;
    }

    private void addJoystick(final int deviceId) {
        mActivity.runOnMainThread(new Runnable() {
            @Override
            public void run() {
                nativeControllerConnected(deviceId);
            }
        });
    }

    private void removeJoystick(final int deviceId) {
        mActivity.runOnMainThread(new Runnable() {
            @Override
            public void run() {
                nativeControllerDisconnected(deviceId);
            }
        });
    }

    private void notifyEngineJoystickEvent(final boolean isLeft, final float x, final float y) {
        mActivity.runOnMainThread(new Runnable() {
            @Override
            public void run() {
                nativeControllerThumbStickEvent(isLeft, x, y);
            }
        });
    }

    private boolean notifyEngineKeyEvent(KeyEvent event, final boolean pressed) {
        final int keyCode = event.getKeyCode();
        boolean handled = true;
        switch (keyCode) {
            case KeyEvent.KEYCODE_BUTTON_R1:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedR1(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_R2:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedR2(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_L1:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedL1(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_L2:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedL2(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_DPAD_LEFT:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedDPadLeft(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedDPadRight(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_DPAD_UP:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedDPadUp(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_DPAD_DOWN:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedDPadDown(pressed);
                    }
                });
                break;
            // ASSUME X PRESSED when KEYCODE_DPAD_CENTER!
            case KeyEvent.KEYCODE_DPAD_CENTER:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedA(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_SELECT:
            case KeyEvent.KEYCODE_BUTTON_START:
            case KeyEvent.KEYCODE_MENU:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedMenu(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_A:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedA(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_Y:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedY(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_X:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedX(pressed);
                    }
                });
                break;
            case KeyEvent.KEYCODE_BUTTON_B:
                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedB(pressed);
                    }
                });
                break;
            default:
                handled = false;
                break;
        }

        return handled;
    }

    private static boolean isFromSource(InputEvent range, int source) {
        return (range.getSource() & source) == source;
    }

    private static boolean isFromSource(InputDevice.MotionRange range, int source) {
        return (range.getSource() & source) == source;
    }

    private InputDeviceState getInputDeviceState(int deviceId) {
        InputDeviceState state = mInputDeviceStates.get(deviceId);
        if (state == null) {
            final InputDevice device = mInputManager.getInputDevice(deviceId);
            if (device == null) {
                return null;
            }
            state = new InputDeviceState(device);
            mInputDeviceStates.put(deviceId, state);
//            MyLog.i(TAG, "Device enumerated: " + state.mDevice);
        }
        return state;
    }

    GamePadHelper(Df3dActivity activity) {
        mActivity = activity;
        mInputManager = (InputManager)activity.getSystemService(Context.INPUT_SERVICE);
        mInputDeviceStates = new SparseArray<>();
    }

    void onPause() {
        mInputManager.unregisterInputDeviceListener(this);
    }

    void onResume() {
        mInputManager.registerInputDeviceListener(this, null);

        // Query all input devices.
        // We do this so that we can see them in the log as they are enumerated.
        int[] ids = mInputManager.getInputDeviceIds();
        for (int id : ids) {
            InputDeviceState state = getInputDeviceState(id);
            if (state != null && deviceIsJoystick(state.getDevice()))
                addJoystick(0);
        }
    }

    @Override
    public void onInputDeviceAdded(int deviceId) {
        InputDeviceState state = getInputDeviceState(deviceId);
        if (state != null) {
            if (deviceIsJoystick(state.getDevice())) {
                addJoystick(0);
            }
        }
    }

    @Override
    public void onInputDeviceRemoved(int deviceId) {
        InputDeviceState state = mInputDeviceStates.get(deviceId);
        if (state != null) {
            if (deviceIsJoystick(state.getDevice())) {
                removeJoystick(0);
            }

            mInputDeviceStates.remove(deviceId);
        }
    }

    @Override
    public void onInputDeviceChanged(int deviceId) {
        InputDeviceState state = mInputDeviceStates.get(deviceId);
        if (state != null) {
            mInputDeviceStates.remove(deviceId);
            getInputDeviceState(deviceId);
        }
    }

    boolean onKeyDown(KeyEvent event) {
        InputDeviceState state = getInputDeviceState(event.getDeviceId());
        if (state != null && state.onKeyDown(event)) {
            return notifyEngineKeyEvent(event, true);
        }

        return false;
    }

    boolean onKeyUp(KeyEvent event) {
        InputDeviceState state = getInputDeviceState(event.getDeviceId());
        if (state != null && state.onKeyUp(event)) {
            return notifyEngineKeyEvent(event, false);
        }

        return false;
    }

    private static class AxisChangeResult
    {
        float value;
        boolean changed;
    }

    AxisChangeResult testFn(int axis, MotionEvent event, float oldValue) {
        AxisChangeResult result = new AxisChangeResult();
        result.changed = false;

        float newValue = event.getAxisValue(axis);
        if (Float.compare(newValue, oldValue) != 0) {
            final InputDevice.MotionRange range =
                    event.getDevice().getMotionRange(axis, event.getSource());

            result.changed = true;
            result.value = 0;

            if (range != null) {
                final float flat = range.getFlat();
                if (Math.abs(newValue) > flat) {
                    result.value = newValue;
                }
            }
        }
        return result;
    }

    void dispatchGenericMotionEvent(MotionEvent event) {
        // Check that the event came from a joystick since a generic motion event
        // could be almost anything.
        if (isFromSource(event, InputDevice.SOURCE_JOYSTICK) && event.getAction() == MotionEvent.ACTION_MOVE) {
            InputDeviceState state = getInputDeviceState(event.getDeviceId());
            if (state != null) {
                boolean leftChanged = false;
                boolean rightChanged = false;

                AxisChangeResult tmp;

                tmp = testFn(MotionEvent.AXIS_X, event, mOldLeftThumbstickX);
                if (tmp.changed) {
                    leftChanged = true;
                    mOldLeftThumbstickX = tmp.value;
                }

                tmp = testFn(MotionEvent.AXIS_Y, event, mOldLeftThumbstickY);
                if (tmp.changed) {
                    leftChanged = true;
                    mOldLeftThumbstickY = tmp.value;
                }

                tmp = testFn(MotionEvent.AXIS_Z, event, mOldRightThumbstickX);
                if (tmp.changed) {
                    rightChanged = true;
                    mOldRightThumbstickX = tmp.value;
                }

                tmp = testFn(MotionEvent.AXIS_RZ, event, mOldRightThumbstickY);
                if (tmp.changed) {
                    rightChanged = true;
                    mOldRightThumbstickY = tmp.value;
                }

                // Left, right triggers
                final float leftTriggerVal = event.getAxisValue(MotionEvent.AXIS_LTRIGGER);
                final float rightTriggerVal = event.getAxisValue(MotionEvent.AXIS_RTRIGGER);

                if (leftChanged) {
                    notifyEngineJoystickEvent(true, mOldLeftThumbstickX, -mOldLeftThumbstickY);
                }
                if (rightChanged) {
                    notifyEngineJoystickEvent(false, mOldRightThumbstickX, -mOldRightThumbstickY);
                }

                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedL2(leftTriggerVal > 0.2f);
                    }
                });

                mActivity.runOnMainThread(new Runnable() {
                    @Override
                    public void run() {
                        nativeControllerButtonPressedR2(rightTriggerVal > 0.2f);
                    }
                });
            }
        }
    }

    private static class InputDeviceState {
        private final InputDevice mDevice;
        private final int[] mAxes;
        private final float[] mAxisValues;
        private final SparseIntArray mKeys;
        InputDeviceState(InputDevice device) {
            mDevice = device;
            int numAxes = 0;
            final List<InputDevice.MotionRange> ranges = device.getMotionRanges();
            for (InputDevice.MotionRange range : ranges) {
                if (isFromSource(range, InputDevice.SOURCE_CLASS_JOYSTICK)) {
                    numAxes += 1;
                }
            }
            mAxes = new int[numAxes];
            mAxisValues = new float[numAxes];
            int i = 0;
            for (InputDevice.MotionRange range : ranges) {
                if (isFromSource(range, InputDevice.SOURCE_CLASS_JOYSTICK)) {
                    mAxes[i++] = range.getAxis();
                }
            }
            mKeys = new SparseIntArray();
        }
        public InputDevice getDevice() {
            return mDevice;
        }

        boolean onKeyDown(KeyEvent event) {
            final int keyCode = event.getKeyCode();
            if (isGameKey(keyCode)) {
                if (event.getRepeatCount() == 0) {
                    mKeys.put(keyCode, 1);
                    return true;
                }
            }
            return false;
        }

        boolean onKeyUp(KeyEvent event) {
            final int keyCode = event.getKeyCode();
            if (isGameKey(keyCode)) {
                int index = mKeys.indexOfKey(keyCode);
                if (index >= 0) {
                    mKeys.put(keyCode, 0);
                    return true;
                }
            }
            return false;
        }

        boolean onJoystickMotion(MotionEvent event) {
            for (int i = 0; i < mAxes.length; i++) {
                final int axis = mAxes[i];
                final float value = event.getAxisValue(axis);
                mAxisValues[i] = value;
            }
            return true;
        }

        // Check whether this is a key we care about.
        // In a real game, we would probably let the user configure which keys to use
        // instead of hardcoding the keys like this.
        private static boolean isGameKey(int keyCode) {
            switch (keyCode) {
                case KeyEvent.KEYCODE_DPAD_UP:
                case KeyEvent.KEYCODE_DPAD_DOWN:
                case KeyEvent.KEYCODE_DPAD_LEFT:
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                case KeyEvent.KEYCODE_DPAD_CENTER:
                case KeyEvent.KEYCODE_SPACE:
                    return true;
                default:
                    return KeyEvent.isGamepadButton(keyCode);
            }
        }
    }
}
