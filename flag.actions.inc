<?php
/**
 * @file
 * Action callbacks for flag module.
 */

/**
 * Implements Drupal action. Flags any entity.
 */
function flag_entity_action(&$entity, $context = array()) {
  $entity_flags = flag_get_flags($context['action_info']['type']);
  if (count($entity_flags) === 1) {
    $flag = reset($entity_flags);
    $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
    $op = ($flag->is_flagged($entity->id()))? 'unflag' : 'flag';
    $flag->flag($op, $entity->id(), $account, TRUE);
  }
  else {
    $entity_info = entity_get_info($context['action_info']['type']);
    $message = 'This action can only flag or unflag items from this interface if there is only a single flag for @label';
    $substitutions = array('@label' => $entity_info['label']);
    backdrop_set_message(t($message, $substitutions), 'warning');
    return FALSE;
  }
}

/**
 * Form for configuring the Flag node action.
 */
function flag_node_action_form($context = array()) {
  return flag_action_form($context, 'node');
}

/**
 * Submit function for the Flag node action form.
 */
function flag_node_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Form for configuring the Flag comment action.
 */
function flag_comment_action_form($context) {
  return flag_action_form($context, 'comment');
}

/**
 * Submit function for the Flag comment action form.
 */
function flag_comment_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Form for configuring the Flag user action.
 */
function flag_user_action_form($context) {
  return flag_action_form($context, 'user');
}

/**
 * Submit function for the Flag user action form.
 */
function flag_user_action_submit($form, $form_state) {
  return flag_action_submit($form, $form_state);
}

/**
 * Generic form for configuring Flag actions.
 *
 * @param $context
 *   The current action context.
 * @param $entity_type
 *   The entity type applicable to this action, such as "node" or "comment".
 */
function flag_action_form($context, $entity_type) {
  $form = array();

  $flags = flag_get_flags($entity_type);
  // If this is a flag_action action, do not allow the triggering flag.
  if (isset($context['actions_flag'])) {
    unset($flags[$context['actions_flag']]);
  }
  $options = backdrop_map_assoc(array_keys($flags));

  $form['flag_action']['#tree'] = TRUE;
  $form['flag_action']['warning'] = array(
    '#markup' => '<div class="messages status">' . t("Note when setting a flag through actions, the selected flag will be flagged regardless of the user's permissions.") . '</div>',
  );
  $form['flag_action']['flag'] = array(
    '#title' => t('Flag to affect'),
    '#type' => 'radios',
    '#options' => $options,
    '#required' => TRUE,
    '#description' => t('When this action is fired, which flag should be flagged (or unflagged)?'),
    '#default_value' => isset($context['flag_action']['flag']) ? $context['flag_action']['flag'] : reset($options),
  );

  $form['flag_action']['op'] = array(
    '#title' => t('Flag operation'),
    '#type' => 'radios',
    '#options' => array('flag' => t('Flag'), 'unflag' => t('Unflag')),
    '#description' => t('When this action is fired, which operation should be performed on the flag?'),
    '#default_value' => isset($context['flag_action']['op']) ? $context['flag_action']['op'] : 'flag',
  );

  if (empty($options)) {
    $error = t('There are no available %type flags. Before you can create an action of this type, you need to <a href="!url">create a %type flag</a>.', array('%type' => $entity_type, '!url' => url(FLAG_ADMIN_PATH . '/add')));
    $form['flag_action']['flag']['#type'] = 'item';
    $form['flag_action']['flag']['#markup'] = $error;
    $form['flag_action']['flag']['#element_validate'][] = 'flag_action_validate_flag';
    $form['flag_action']['flag']['#flag_error'] = $error;
  }

  return $form;
}

/**
 * Generic validation handler for validating Flag action configuration.
 */
function flag_action_validate_flag($element) {
  if (isset($element['#flag_error'])) {
    form_error($element, $element['#flag_error']);
  }
}

/**
 * Generic submission handler for saving Flag action configuration.
 */
function flag_action_submit($form, $form_state) {
  return array(
    'flag_action' => $form_state['values']['flag_action'],
  );
}
