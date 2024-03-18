export default defineComponent(({ name, template, schema }) => {
	name('genshicraft:food')
	schema({
		description: 'The component for GenshiCraft food.'
	})

	template(({ }, { create }) => {
		create(
			{
				'minecraft:creative_category': {
					'parent': 'itemGroup.name.GenshiCraft.Food'
				},
				'minecraft:food': {
					'can_always_eat': true
				},
				'minecraft:render_offsets': {
					'main_hand': {
						'first_person': {
							'scale': [
								0.003,
								0.003,
								0.003
							]
						},
						'third_person': {
							'scale': [
								0.006,
								0.006,
								0.006
							]
						}
					}
				},
				'minecraft:use_duration': 0.1
			},
			'minecraft:item/components'
		)
	})
})
